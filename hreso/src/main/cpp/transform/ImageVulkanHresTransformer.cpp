//
// Created by asus on 2022/11/17.
//

#include "ImageVulkanHresTransformer.h"

ImageVulkanHresTransformer::ImageVulkanHresTransformer(): engine(make_shared<VKEngineRenderer>()),
                                                        window(nullptr),options(nullptr), image(
                nullptr) {

}

ImageVulkanHresTransformer::~ImageVulkanHresTransformer() {
    release();
}

void ImageVulkanHresTransformer::setWindow(ANativeWindow *window) {
    this->window = window;
}

void ImageVulkanHresTransformer::updateViewPoint(int width, int height) {
    this->width = width;
    this->height = height;
//    if (engine != nullptr && this->window != nullptr) {
//        engine->initWindow(window, width, height);
//    } else {
//        HLOGE("updateViewPoint, window is null");
//    }
}

void ImageVulkanHresTransformer::transformOption(IOptions *options) {
    if (options != nullptr) {
        image = LoadTextureUtil::loadImageFromOption(options);
        this->options = options;
    } else {
        HLOGE("transformOption, option is null");
    }
}

void ImageVulkanHresTransformer::setListener(FilterListener *listener) {
    this->listener = listener;
}

void ImageVulkanHresTransformer::transform() {
    if (engine == nullptr) {
        HLOGE("transform, engine is null");
        return;
    }

    if (image == nullptr) {
        HLOGE("transform,image data is null");
        return;
    }

    if (options == nullptr) {
        HLOGE("transform, option is null");
        return;
    }

    if (options->srcWidth <= 0 || options->srcHeight <= 0 || options->srcChannel <= 0) {
        HLOGE("transform, option not work, %i, %i, %i", options->srcWidth, options->srcHeight, options->srcChannel);
        return;
    }
    HLOGV("transform, width %i, height %i, channel %i", options->srcWidth, options->srcHeight, options->srcChannel);
//    long length = options->srcWidth * options->srcHeight * options->srcChannel;
    if (this->window != nullptr) {
        HLOGV("transform, initWindow");
        engine->initWindow(window, options);
        engine->setOption(options);
        engine->setListener(listener);
        engine->drawImg();
    } else {
        HLOGV("transform, initOffscreen");
        engine->initOffscreen(options);
        engine->setListener(listener);
        engine->drawOffscreenImg();
    }

    //释放图片内存
    LoadTextureUtil::releaseImage(image);
}

void ImageVulkanHresTransformer::release() {
    engine = nullptr;
    options = nullptr;
    listener = nullptr;
}
