package com.werb.pickphotoview.widget

import android.content.Context
import android.graphics.PorterDuff
import android.net.Uri
import android.util.AttributeSet
import android.view.LayoutInflater
import android.view.View
import android.widget.FrameLayout
import android.widget.RelativeLayout
import android.widget.Toast
import com.bumptech.glide.Glide
import com.werb.eventbus.EventBus
import com.werb.pickphotoview.GlobalData
import com.werb.pickphotoview.R
import com.werb.pickphotoview.event.PickPreviewEvent
import com.werb.pickphotoview.extensions.color
import com.werb.pickphotoview.extensions.drawable
import com.werb.pickphotoview.extensions.string
import com.werb.pickphotoview.model.MediaModel
import com.werb.pickphotoview.model.SelectModel
import com.werb.pickphotoview.util.PickPhotoHelper
import kotlinx.android.synthetic.main.pick_widget_view_preview.view.*

/** Created by wanbo <werbhelius@gmail.com> on 2017/10/19. */

class PreviewImage : FrameLayout {

    private val images = PickPhotoHelper.selectImages

    constructor(context: Context) : super(context)
    constructor(context: Context, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr)

    init {
        initView()
    }

    private fun initView() {
        val layout = LayoutInflater.from(context).inflate(R.layout.pick_widget_view_preview, this, false) as RelativeLayout
        addView(layout)
    }

    fun setImage(model: MediaModel, full: () -> Unit) {
        select(model)
        image.setOnClickListener { full() }
        image.isDrawingCacheEnabled = true
        selectLayout.setOnClickListener {
            if (images.contains(SelectModel(model.path, model.duration > 0L))) {
                removeImage(model)
            } else {
                GlobalData.model?.let {
                    if (it.allPhotoSize != 0) {
                        val pickSize = images.size + it.hasPhotoSize
                        if (pickSize >= it.allPhotoSize) {
                            Toast.makeText(context, String.format(context.string(R.string.pick_photo_size_limit), it.allPhotoSize.toString()), Toast.LENGTH_SHORT).show()
                            return@setOnClickListener
                        }
                    }
                    val pickSize = images.size
                    if (pickSize >= it.pickPhotoSize) {
                        Toast.makeText(context, String.format(context.string(R.string.pick_photo_size_limit), it.pickPhotoSize.toString()), Toast.LENGTH_SHORT).show()
                        return@setOnClickListener
                    }
                    addImage(model)
                }
            }
        }
        Glide.with(context)
                .load(Uri.parse("file://" + model.path))
                .thumbnail(.1f)
                .into(image)
    }

    fun clear() {
        Glide.with(context).clear(image)
    }

    /** add image in list */
    private fun addImage(model: MediaModel) {
        images.add(SelectModel(model.path, model.duration > 0L))
        select(model)
        EventBus.post(PickPreviewEvent(model.path))
    }

    /** remove image in list */
    private fun removeImage(model: MediaModel) {
        images.remove(SelectModel(model.path, model.duration > 0L))
        select(model)
        EventBus.post(PickPreviewEvent(model.path))
    }

    private fun select(model: MediaModel) {
        if (images.contains(SelectModel(model.path, model.duration > 0L))) {
            check.visibility = View.VISIBLE
            selectBack.visibility = View.VISIBLE
            val drawable = context.drawable(R.drawable.pick_svg_select_select)
            val back = context.drawable(R.drawable.pick_svg_select_back)
            GlobalData.model?.selectIconColor?.let {
                back.setColorFilter(context.color(it), PorterDuff.Mode.SRC_IN)
            }
            selectLayout.background = drawable
            selectBack.background = back
        } else {
            check.visibility = View.GONE
            selectBack.visibility = View.GONE
            val drawable = context.drawable(R.drawable.pick_svg_select_default)
            selectLayout.setBackgroundDrawable(drawable)
        }

    }
}