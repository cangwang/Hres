package com.cangwang.hreso.util

import android.view.Surface
import com.cangwang.hreso.bean.ImageOptionParams
import com.cangwang.hreso.impl.HresListener

object HresJniUtil {

    init {
        System.loadLibrary("hres")
    }

    external fun nativeCreateTransformer(tag: String, engineOptionParams: String, surfaceTexture: Surface?)

    external fun nativeUpdateViewPoint(width: Int, height: Int)

    external fun nativeTransform(option: String, op: ImageOptionParams)

    external fun nativeSetListener(listener: HresListener)

    external fun nativeTransformRelease()
}