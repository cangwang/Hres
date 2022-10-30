package com.cangwang.hreso.impl

import com.cangwang.hreso.bean.OptionParams

interface HresListener {

    fun hresTransformStart(option: OptionParams)

    fun hresTransformComplete(option: OptionParams)

    fun hresTransformComplete()

    fun hresTransformError(option: OptionParams, errorTag: String)

}