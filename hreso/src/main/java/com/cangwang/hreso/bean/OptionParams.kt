package com.cangwang.hreso.bean

import org.json.JSONObject

class OptionParams {
    var name = ""
    var address = ""
    var saveAddress = ""
    var filterType = "simple"  //滤镜名称
    var type = 1  // 1是图片 2是视频
    var scaleType = 1  //1 使用scaleRatio, 2使用固定宽高
    var scaleRatio = 1f
    var scaleWidth = 0f
    var scaleHeight = 0f
    var front = false //是否插入队头
    var async = false //是否同步转换

    fun toJson(): String {
        val json = JSONObject()
        json.put("name", name)
        json.put("address", address)
        json.put("saveAddress", saveAddress)
        json.put("type", type)
        json.put("filterType", filterType)
        json.put("scaleType", scaleType)
        json.put("scaleRatio", scaleRatio)
        json.put("scaleWidth", scaleWidth)
        json.put("scaleHeight", scaleHeight)
        json.put("front", front)
        json.put("async", async)
        return json.toString()
    }

    fun parse(option: String) {
        val json = JSONObject(option)
        name = json.optString("name")
        address = json.optString("address")
        saveAddress = json.optString("saveAddress")
        filterType = json.optString("filterType")
        type = json.optInt("type")
        scaleType = json.optInt("scaleType")
        scaleRatio = json.optDouble("scaleRatio").toFloat()
        scaleWidth = json.optDouble("scaleRatio").toFloat()
        scaleHeight = json.optDouble("scaleHeight").toFloat()
        front = json.optBoolean("front")
        async = json.optBoolean("async")
    }
}