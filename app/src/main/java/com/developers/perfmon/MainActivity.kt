package com.developers.perfmon

import android.content.Intent
import android.os.Bundle
import android.os.Looper
import android.os.Process
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import kotlin.concurrent.thread


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        button.setOnClickListener {
            thread {
                val cpuTime = getCpuTimeFromProc(Process.myPid().toLong())
                Log.d("Initial CPU Time", "Initial value from Proc: $cpuTime")
            }
            val cpuTime = getCpuTime()
            Log.d("Initial CPU Time", "Initial value from rusage: $cpuTime")
            startActivity(Intent(this, DetailsActivity::class.java))
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