package com.cangwang.hreso.bean

import org.json.JSONObject

/**
 * engine setting option
 */
class EngineOptionParams {
    var useVulkan = true
    var transformType = "img" //img video

    fun toJson(): String {
        val json = JSONObject()
        json.put("useVulkan", useVulkan)
        json.put("transformType", transformType)
        return json.toString()
    }

    override fun toString(): String {
        return "EngineOptionParams(useVulkan=$useVulkan, transformType='$transformType')"
    }

}