# Perfmon
A playground to calculate CPU time from `getrusage` and Proc file system. 

CPU time consists of two values user CPU time and system CPU time:
* User CPU time: Time spent to execute your code
* System CPU time: Time spent to execute system calls

To know further about CPU time and how it is useful, read [this](https://www.amanjeet.me/three-musketeers-of-time/) blog which will tell you about how to use these values, why are they useful and some other interesting facts.

## getrusage
This struct API can help you to get the CPU time of a whole process or thread, based on a parameter value passed in the function `getrusage`. This API will help you return both user CPU time and system CPU time which can be summed to calculate the CPU time value. Lets look at the usage of this API:
	
* **Getting User Time and System Time**: The first parameter of `getrusage` specifies the task for which you want to calculate the resource usage. In this case we passed `RUSAGE_THREAD` which means we want to calculate it for the **calling thread** which would be main thread in the case where you want to optimize for some rendering issues. You can look at more possible values for this constant in this [man](https://man7.org/linux/man-pages/man2/getrusage.2.html) page.
		
```C++
#include <sys/resource.h>
		
rusage rusageStats{};
getrusage(RUSAGE_THREAD, &rusageStats);
		
user_cpu_time = rusageStats.ru_utime
system_cpu_time = rusageStats.ru_stime
```
              
* The values returned in the previous section are in [timval](https://man7.org/linux/man-pages/man2/gettimeofday.2.html) structure which is basically seconds plus microseconds. To convert each valiue from *timval to milliseconds* we do following:
                
```C++
static constexpr auto kMillisInSec = 1000;
static constexpr auto kMicrosInMillis = 1000;
		
inline uint64_t timeval_sum_to_millis(timeval &tv1, timeval &tv2){
  return (tv1.tv_sec + tv2.tv_sec) * kMillisInSec +
      (tv1.tv_usec + tv2.tv_usec) / kMicrosInMillis;
}
		
// CPU TIME can thus be determined by
uint64_t cpu_time_millis = timeval_sum_to_millis(
    rusageStats.ru_utime,
    rusageStats.ru_stime
);
```
Addition of the two values user CPU time and system CPU time will finally give you the CPU time.

* Interface the above C++ code to Java/Kotlin through a JNI method to obtain this sum. You can see details of how to implement simple JNI method [here](https://developer.android.com/ndk/samples/sample_hellojni).
* ***Ensure you call this JNI method on main thread*** because we have used `RUSAGE_THREAD` in our `getrusage`parameter which will evaluate CPU time for the calling thread (main thread). Calling it from another worker thread will give you values for the other thread and not the expected main thread.

## Proc File System
One of the disadvantages using `getrusage` is that we need to call it from the current profiling task, which induces an [***observer effect***](https://en.wikipedia.org/wiki/Observer_effect_(physics)#:~:text=In%20physics%2C%20the%20observer%20effect,they%20measure%20in%20some%20manner.). This means, that you might end up interfering the original values. Thus we need, such an API which would be independent from such *contexts* of thread or process and would not interfere with original values.

Proc file system is a virtual file system in linux which can provide you the information about different tasks (process and threads) currently running on the system.

For the CPU time of our main thread, we must get the content of the file which are mapped directly to our main thread in proc file system.

Lets divide this approach to the following parts and dive into it one by one:

1. [Identifying the proc file for Main Thread](#identifying-the-proc-file-for-main-thread)
2. [Reading the Main thread Proc File](#reading-the-main-thread-proc-stat-file)
3. [Parsing the Main thread Proc File](#parsing-the-main-thread-proc-file)
4. [Calculating CPU time from stat file](#calculating-cpu-time-from-stat)

### Identifying the proc file for Main Thread

All the processes and threads in linux have a directory mapping in proc file system. For referencing the proc directory of current process you must know your process id (pid). This can be obtained easily by `Process.myPid()`.

All the processes in proc have a numeric directory and the threads of process gets mapped to their thread id (tid) under a `task` directory as shown below:

<p align="center">
<img src= "https://user-images.githubusercontent.com/12881364/126125996-64b1e1b0-6301-4bdc-879a-336ce8166671.jpeg" width="700px"/>
</p>


#### cd process

To reference the current process you need to reference `proc/self` directory which is nothing but a [symbolic link](https://linuxize.com/post/how-to-create-symbolic-links-in-linux-using-the-ln-command/) (shortcut) to your process directory in proc.

#### cd process/thread

The main thread has an id equal to that of pid because it is one of the first thread to get created from the process. You can check this by calling `gettid()` from main thread. Thus, to reach the main thread directory you need to go to `proc/self/task/<tid>`.

#### cd process/thread/stat

Every directory has a stat file which holds status information of the task which has the two entries that we need to calculate the CPU time: **user CPU time** and **system CPU time**. So, the path to this file for main thread id 24484 would be: `proc/self/task/24484/stat`.

This file gives you status of many information including: current state of task, name of task, priority etc. Its a raw dump of values in order defined in [man page](https://man7.org/linux/man-pages/man5/proc.5.html) of proc. This dump looks like as follows:

```
24484 (elopers.perfmon) R 1435 1435 0 0 -1 4219200 7210 0 0 0 16 17 0.....
``` 

Here, the first entry is thread id, second entry is name and third entry is the current state of thread. 

You can also look out for the exact order in this [stackoverflow answer](https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat).

Our objective would be to look up the `utime` and `stime` and then from these values we will evaluate the main thread CPU time.


### Reading the Main thread Proc stat File

From the above discussion, our main thread proc file is: `proc/self/task/24484/stat`.

For opening this file I have used `open` function from standard library header `fcnt.h` (file control). They are readily available as part of standard `libc`. Let's first look at the code snippet for opening stat file:

```C++
int ProcFs::openStatFile(const std::string &path) {
	
	 // 1	
    int statFile = open(path.c_str(), O_SYNC | O_RDONLY);

    // Could'nt open stat file from proc
    if (statFile == -1) {
        throw std::system_error(
                errno, std::system_category(), "Could not open stat file");
    }
    return statFile;
}
```
The `open` function at **`1`** opens file at `path` in read only mode and returns a file descriptor which will be used by further `read` and `lseek` calls. 

For reading the contents of file I have used `read` function from `unistd.h` that are also readily available in UNIX distributions. Let's see how to use this `read` function in our use case:

```C++
// Read proc stat file
constexpr size_t bufferLength = 512;
char buffer[bufferLength]{};
int bytes_read = read(fd, buffer, (sizeof(buffer) - 1));
if (bytes_read < 0) {
    throw std::system_error(
        errno,
        std::system_category(),
        "Could not read stat file"
    );
}
```
The above code will read the contents of stat file in the char `buffer`.

We now have the contents of the stat file in buffer. Let's, move on to the parsing of these files.

### Parsing the Main thread Proc File 

For parsing the file I have created a `skipUntil` function which uses one of its argument (`ch`) as delimiter to parse one entry in the file. Let's have a look on this function:

```C++
char *skipUntil(char *data, const char *end, char ch) {
  // It's important that we check against `end`
  // before we dereference `data`.
  while (data < end && *data != ch) {
    if (*data == '\0') {
        throw std::runtime_error("Unexpected end of string");
    }

    ++data;
  }

  if (data == end) {
    throw std::runtime_error("Unexpected end of string");
  }

  // One past the `ch` character.
  return ++data;
}
```

So, for reading the `24484 (elopers.perfmon) R ` we will do:

``` C++
data = skipUntil(data, end, ' '); // pid
data = skipUntil(data, end, ')'); // name
data = skipUntil(data, end, ' '); // space after name
```

To keep record of important readings I created a struct called as `TaskStatInfo` which will hold all important information.

### Calculating CPU time from stat 

Let's assume you parsed the whole file according to above strategy. We will calculate CPU time as follows:

```C++
static int kClockTicksMs = systemClockTickIntervalMs();

TaskStatInfo info{};
info.cpuTime = kClockTicksMs * (utime + stime);
``` 
The `utime` and `stime` you directly get from proc file are not in millisecond. They are universally measured in linux with clock ticks. 1 tick corresponds to 10 ms. 

The `kClockTicksMs` from `systemClockTickIntervalMs()` takes care of conversion of those ticks into milliseconds. Hence, we will get `info.cpuTime` in ms.

You can now obtain these values by integrating a JNI function and call it from any background thread **without interfering the original profile of Main thread**. For me these values were:

```
D/Initial CPU Time: Initial value from Proc: 300
D/Final CPU Time: Final value from Proc: 1120

(Net CPU time: 1120 - 300 = 820ms)
```

The above aproaches can be used to get CPU time from the production environment and can be userful to point performance issues from production. 
