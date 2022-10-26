package com.cangwang.hres

import android.Manifest
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.content.PermissionChecker
import com.werb.pickphotoview.PickPhotoView
import com.werb.pickphotoview.model.SelectModel
import com.werb.pickphotoview.util.PickConfig
import kotlinx.android.synthetic.main.activity_main.*
import java.util.ArrayList

class MainActivity : AppCompatActivity() {

    private var CAMERA_PERMISSION_REQ = 1

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.CAMERA) == PermissionChecker.PERMISSION_DENIED ||
            PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PermissionChecker.PERMISSION_DENIED) {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.CAMERA,Manifest.permission.WRITE_EXTERNAL_STORAGE), CAMERA_PERMISSION_REQ)
        } else {
            initView()
        }
    }

    fun initView() {
        button_album.setOnClickListener {
            PickPhotoView.Builder(this@MainActivity)
                .setPickPhotoSize(1)
                .setClickSelectable(true)             // click one image immediately close and return image
                .setShowCamera(true)
                .setHasPhotoSize(7)
                .setAllPhotoSize(10)
                .setSpanCount(3)
                .setLightStatusBar(false)
                .setShowGif(false)                    // is show gif
                .setShowVideo(false)
                .start()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        if (requestCode == CAMERA_PERMISSION_REQ &&(grantResults.size != 1 || grantResults[0] == PermissionChecker.PERMISSION_GRANTED)) {
            initView()
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode == 0) {
            return
        }
        if (data == null) {
            return
        }
        if (requestCode == PickConfig.PICK_PHOTO_DATA) {
            val selectPaths = data.getSerializableExtra(PickConfig.INTENT_IMG_LIST_SELECT) as ArrayList<SelectModel>
            if (selectPaths.size>0) {
                val intent = Intent(this, ImageTransformActivity::class.java)
                intent.putExtra(PickConfig.INTENT_IMG_LIST_SELECT, selectPaths[0])
                startActivity(intent)
            }else{
                Toast.makeText(this,"choose no picture", Toast.LENGTH_SHORT).show()
            }
        }
    }
}