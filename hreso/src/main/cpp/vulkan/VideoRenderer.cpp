//
// Created by asus on 2022/11/7.
//

#include "VideoRenderer.h"

#include <vulkan/engine/VKEngineRenderer.h>

VideoRenderer::VideoRenderer(): m_width(0)
        , m_height(0)
        , m_backingWidth(0)
        , m_backingHeight(0)
        , m_params(0)
        , m_rotation(0)
        , isDirty(false)
        , isProgramChanged(false)
{

}

VideoRenderer::~VideoRenderer() {

}

unique_ptr<VideoRenderer> VideoRenderer::create(int type) {
    return unique_ptr<VideoRenderer>(make_unique<VKEngineRenderer>());
}