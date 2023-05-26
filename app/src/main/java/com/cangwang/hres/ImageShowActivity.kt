package com.cangwang.hres

import android.graphics.SurfaceTexture
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.Surface
import android.view.TextureView
import android.view.Window
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import com.cangwang.hreso.bean.EngineOptionParams
import com.cangwang.hreso.bean.ImageOptionParams
import com.cangwang.hreso.impl.HresListener
import com.cangwang.hreso.util.HresJniUtil
import com.werb.pickphotoview.model.SelectModel
import com.werb.pickphotoview.util.PickConfig
import kotlinx.android.synthetic.main.activity_show.*

class ImageShowActivity: AppCompatActivity(), TextureView.SurfaceTextureListener {
    val TAG = ImageShowActivity::class.java.simpleName
    var selectPaths: SelectModel? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.activity_show)

        selectPaths = intent.getSerializableExtra(PickConfig.INTENT_IMG_LIST_SELECT) as SelectModel
        textureView.surfaceTextureListener = this
    }


    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        val s = Surface(surface)
        val engineOption = EngineOptionParams()
        HresJniUtil.nativeCreateTransformer("ImageShow", engineOption.toJson(), s)
        HresJniUtil.nativeUpdateViewPoint(width, height)
        HresJniUtil.nativeSetListener(object : HresListener {
            override fun hresTransformStart(imageOption: ImageOptionParams) {

            }

            override fun hresTransformComplete(imageOption: ImageOptionParams) {
//                val desBitmap = BitmapFactory.decodeFile(option.saveAddress)
//                btn_album_transform.setImageBitmap(desBitmap)
            }

            override fun hresTransformError(imageOption: ImageOptionParams, errorTag: String) {
                Log.e(TAG, "$errorTag $imageOption")
            }
        })
        val option = ImageOptionParams()
        option.address = selectPaths!!.path
        option.scaleRatio = 1.0f
        option.filterType = "fsrup"
        val sList = option.address.split(".")
        val dex = sList[sList.size - 1]
        val saveAddress = if (Build.BRAND == "Xiaomi"){ // 小米手机
            "${Environment.getExternalStorageDirectory().path}/DCIM/Camera/${System.currentTimeMillis()}.${dex}"
        }else{  // Meizu 、Oppo
            "${Environment.getExternalStorageDirectory().path}/DCIM/${System.currentTimeMillis()}.${dex}"
        }
        option.saveAddress = saveAddress
        HresJniUtil.nativeTransform(option.toJson(), option)
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {

    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        HresJniUtil.nativeTransformRelease()
        surface.release()
        return true
    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {

    }
}