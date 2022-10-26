package com.cangwang.hreso.util

import com.cangwang.hreso.impl.HresListener

object HresJniUtil {

    init {
        System.loadLibrary("hres")
    }

    external fun nativeCreateTransformer(tag: String, optionParams: String)

    external fun nativeTransform(option: String)

    external fun nativeSetListener(listener: HresListener)

    external fun nativeTransformAsync(option: String)

    external fun nativeTransformRelease()
}