package com.cangwang.hreso.bean

import org.json.JSONObject

class OptionParams {
    var address = ""
    var scaleRatio = 1f
    var scaleWidth = 0f
    var scaleHeight = 0f
    var async = false

    fun toJson(): String {
        val json = JSONObject()
        json.put("address", address)
        json.put("scaleRatio", scaleRatio)
        json.put("scaleWidth", scaleWidth)
        json.put("scaleHeight", scaleHeight)
        json.put("async", async)
        return json.toString()
    }
}