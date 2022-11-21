package com.cangwang.hreso.impl

import com.cangwang.hreso.bean.ImageOptionParams

interface HresListener {

    fun hresTransformStart(imageOption: ImageOptionParams)

    fun hresTransformComplete(imageOption: ImageOptionParams)

    fun hresTransformError(imageOption: ImageOptionParams, errorTag: String)

}