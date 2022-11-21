package com.cangwang.hreso.bean

import org.json.JSONObject

/**
 * image setting option
 */
class ImageOptionParams {
    var name = ""
    var address = ""
    var saveAddress = ""
    var filterType = "fsr"  //滤镜名称
    var type = 1  // 1是图片 2是视频
    var scaleType = 1  //1 使用scaleRatio, 2使用固定宽高
    var scaleRatio = 1f //缩放倍数
    var scaleWidth = 0f  //缩放后的宽
    var scaleHeight = 0f  //缩放后的高
    var rotation = 0   //图片角度
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
        json.put("rotation", rotation)
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

    override fun toString(): String {
        return "OptionParams(name='$name', address='$address', saveAddress='$saveAddress', " +
                "filterType='$filterType', type=$type, scaleType=$scaleType, " +
                "scaleRatio=$scaleRatio, scaleWidth=$scaleWidth, scaleHeight=$scaleHeight, " +
                "rotation = $rotation, front=$front, async=$async)"
    }
}