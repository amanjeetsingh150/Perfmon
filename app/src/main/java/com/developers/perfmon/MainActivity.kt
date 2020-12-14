package com.developers.perfmon

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        button.setOnClickListener {
            val cpuTime = getCpuTime()
            Log.d("Initial CPU Time", "Initial value: $cpuTime")
            startActivity(Intent(this, DetailsActivity::class.java))
        }
    }

    private external fun getCpuTime(): Long

    companion object {
        private const val PERF_LIB_NAME = "perf"

        init {
            System.loadLibrary(PERF_LIB_NAME)
        }
    }
}