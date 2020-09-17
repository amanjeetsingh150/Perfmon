package com.developers.perfmon

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        button.setOnClickListener {
            getSysInfo()
        }
    }

    private external fun getSysInfo(): String

    companion object {
        private const val PERF_LIB_NAME = "perf"

        init {
            System.loadLibrary(PERF_LIB_NAME)
        }
    }
}