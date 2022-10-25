package com.cangwang.hreso.impl

interface HresListener {

    fun hresTransformStart(option: String)

    fun hresTransformComplete(option: String)

    fun hresTransformError(option: String, errorTag: String)

}