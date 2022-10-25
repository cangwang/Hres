package com.cangwang.hreso.util

import android.graphics.Bitmap
import com.cangwang.hreso.impl.HresListener

object HresJniUtil {

    init {
        System.loadLibrary("hres")
    }

    external fun nativeCreateTransformer(tag: String, optionParams: String)

    external fun nativeTransform(option: String)

    external fun nativeTransformAsync(option: String, listener: HresListener)

    external fun nativeTransformRelease()
}