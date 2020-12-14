package com.developers.perfmon

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.developers.perfmon.util.LoadTestObjects


class DetailsActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        LoadTestObjects.load(10000000L)
        setContentView(R.layout.activity_details)
    }

    override fun onStart() {
        super.onStart()
        val cpuTime = getCpuTime()
        Log.d("Final CPU Time", "$cpuTime")
    }

    private external fun getCpuTime(): Long

    companion object {
        private const val PERF_LIB_NAME = "perf"

        init {
            System.loadLibrary(PERF_LIB_NAME)
        }
    }
}