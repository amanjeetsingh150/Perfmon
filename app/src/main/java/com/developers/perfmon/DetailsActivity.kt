package com.developers.perfmon

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Process
import android.util.Log
import com.developers.perfmon.util.LoadTestObjects
import kotlin.concurrent.thread


class DetailsActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        LoadTestObjects.load(10000000L)
        setContentView(R.layout.activity_details)
    }

    override fun onStart() {
        super.onStart()
        val cpuTime = getCpuTime()
        Log.d("Final CPU Time", "Final value from rusage: $cpuTime")
        thread {
            val cpuTimeFromProc = getCpuTimeFromProc(Process.myPid().toLong())
            Log.d("Final CPU Time", "Final value from Proc: $cpuTimeFromProc")
        }
    }

    private external fun getCpuTime(): Long

    private external fun getCpuTimeFromProc(threadId: Long): Long

    companion object {
        private const val PERF_LIB_NAME = "perf"

        init {
            System.loadLibrary(PERF_LIB_NAME)
        }
    }
}