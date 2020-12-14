package com.developers.perfmon.util

import com.developers.perfmon.Test

object LoadTestObjects {

    fun load(count: Long) {
        for (i in 1..count) {
            Test("Test Key $i", i.toInt())
        }
    }

}