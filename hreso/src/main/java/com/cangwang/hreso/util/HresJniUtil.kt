package com.cangwang.hreso.util

import android.view.Surface
import com.cangwang.hreso.bean.OptionParams
import com.cangwang.hreso.impl.HresListener

object HresJniUtil {

    init {
        System.loadLibrary("hres")
    }

    external fun nativeCreateTransformer(tag: String, optionParams: String, surfaceTexture: Surface?)

    external fun nativeTransform(option: String, op: OptionParams)

    external fun nativeSetListener(listener: HresListener)

    external fun nativeTransformRelease()
}