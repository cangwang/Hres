package com.cangwang.hreso.util

import android.graphics.Bitmap

object HresJniUtil {

    init {
        System.loadLibrary("hres")
    }

    external fun nativeCreateTransformer(tag: String, optionParams: String)

    external fun nativeTransform(address: String): Bitmap

    external fun nativeTransformBitmap(bitmap: Bitmap): Bitmap

    external fun nativeTransformAsync(bitmap: Bitmap, listener: Bitmap)

    external fun nativeTransformRelease()
}