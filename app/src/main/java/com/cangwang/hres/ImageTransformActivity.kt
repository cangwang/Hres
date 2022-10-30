package com.cangwang.hres

import android.graphics.BitmapFactory
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.Window
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import com.cangwang.hreso.bean.OptionParams
import com.cangwang.hreso.impl.HresListener
import com.cangwang.hreso.util.HresJniUtil
import com.werb.pickphotoview.model.SelectModel
import com.werb.pickphotoview.util.PickConfig
import kotlinx.android.synthetic.main.activity_album.*

class ImageTransformActivity: AppCompatActivity() {

    private val TAG = ImageTransformActivity::class.java.simpleName

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.activity_album)
        val selectPaths = intent.getSerializableExtra(PickConfig.INTENT_IMG_LIST_SELECT) as SelectModel
        val srcBitmap = BitmapFactory.decodeFile(selectPaths.path)
        btn_album_src.setImageBitmap(srcBitmap)
        HresJniUtil.nativeCreateTransformer("image", "")
        HresJniUtil.nativeSetListener(object : HresListener {
            override fun hresTransformStart(option: OptionParams) {

            }

            override fun hresTransformComplete(option: OptionParams) {
                val desBitmap = BitmapFactory.decodeFile(option.saveAddress)
//                btn_album_transform.setImageBitmap(desBitmap)
            }

            override fun hresTransformComplete() {
            }

            override fun hresTransformError(option: OptionParams, errorTag: String) {
                Log.e(TAG, "$errorTag $option")
            }
        })
        val option = OptionParams()
        option.address = selectPaths.path
        option.scaleRatio = 1.0f
        val saveAddress = if(Build.BRAND == "Xiaomi"){ // 小米手机
            Environment.getExternalStorageDirectory().path +"/DCIM/Camera/"+System.currentTimeMillis()+".png"
        }else{  // Meizu 、Oppo
            Environment.getExternalStorageDirectory().path +"/DCIM/"+System.currentTimeMillis()+".png"
        }
        option.saveAddress = saveAddress
        HresJniUtil.nativeTransform(option.toJson(), option)
    }


    override fun onDestroy() {
        super.onDestroy()
        HresJniUtil.nativeTransformRelease()
    }
}