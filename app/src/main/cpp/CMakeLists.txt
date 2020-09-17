cmake_minimum_required(VERSION 3.4.1)

add_library(perf SHARED
            perf.cpp)

# Include libraries needed for hello-jni lib
target_link_libraries(perf
                      android
                      log)