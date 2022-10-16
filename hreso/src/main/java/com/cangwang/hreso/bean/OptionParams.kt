package com.cangwang.hreso.bean

import org.json.JSONObject

class OptionParams {
    var name = ""
    var address = ""
    var filterType = "simple"  //滤镜名称
    var type = 1  // 1是图片 2是视频
    var scaleType = 1  //1 使用scaleRatio, 2使用固定宽高
    var scaleRatio = 1f
    var scaleWidth = 0f
    var scaleHeight = 0f
    var async = false

    fun toJson(): String {
        val json = JSONObject()
        json.put("name", name)
        json.put("address", address)
        json.put("type", type)
        json.put("filterType", filterType)
        json.put("scaleType", scaleType)
        json.put("scaleRatio", scaleRatio)
        json.put("scaleWidth", scaleWidth)
        json.put("scaleHeight", scaleHeight)
        json.put("async", async)
        return json.toString()
    }
}