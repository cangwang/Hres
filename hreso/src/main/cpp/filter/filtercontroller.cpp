//
// Created by cangwang on 2022/10/22.
//

#include "filtercontroller.h"

FilterController::FilterController() {

}

FilterController::~FilterController() {
    filterList.clear();
    if (eglCore != nullptr) {
        eglCore->release();
    }
    if (saveImgData != nullptr) {
        saveImgData = nullptr;
    }

    destroyPixelBuffers();
    option = nullptr;
}

void FilterController::transformFilter(IOptions *option) {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start();
    }
    if (option == nullptr) {
        HLOGV("transformFilter optioni nullptr");
        return;
    }
    if (filterName != option->getName()) {  //滤镜模式不一样
        if (option->getName() == "simple") {
            filterList.push_front(new SimpleFilter());
        }
        filterName = option->getName();
    }
    LoadTextureUtil::loadTextureFromFile(option);
    this->option = option;
}

void FilterController::render() {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start();
    }
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    initPixelBuffer();
    if (!filterList.empty()) {
        for (IFilter* filter: filterList) {
            filter->renderFrame();
        }
    }

    if (eglCore != nullptr) {
        eglCore->swapBuffer();
    }
    drawPixelBuffer();
    if (option != nullptr) {  //保存图片
        save(option);
    }
}

void FilterController::save(IOptions* option) {
    string address;
    if (option->getSaveAddress().empty()) {
        HLOGV("saveAddress is null, use origin address");
        address = option->getAddress();
    } else {
        address = option->getSaveAddress();
    }
    if (scaleImgWidth > 0 && scaleImgHeight > 0) {
        bool result = saveImg(address, saveImgData, scaleImgWidth, scaleImgHeight,
                              option->srcChannel);

        if (listener != nullptr) {
            if (result) {
                listener->filterRenderComplete(option);
            } else {
                listener->filterRenderFail(option, string("saveImage fail"));
            }
        }
    } else {
        if (listener != nullptr) {
            listener->filterRenderFail(option, string("scaleImgWidth || scaleImgHeight = 0"));
        }
    }
}

bool FilterController::saveImg(const string& saveFileAddress,unsigned char* data,int width,int height, int channel) {
    //屏幕到文件保存需要使用
    stbi_flip_vertically_on_write(1);
    //保存图片到本地文件
    if (channel == 3) {
        if (stbi_write_jpg(saveFileAddress.c_str(), width, height, channel, data, 0)) {
            HLOGV("save address = %s success", saveFileAddress.c_str());
            free(data);
            return true;
        } else {
            free(data);
            HLOGE("save fail address = %s fail", saveFileAddress.c_str());
            return false;
        }
    } else if (channel == 4) {
        if (stbi_write_png(saveFileAddress.c_str(), width, height, channel, data, 0)) {
            HLOGV("save address = %s success", saveFileAddress.c_str());
            free(data);
            return true;
        } else {
            free(data);
            HLOGE("save fail address = %s fail", saveFileAddress.c_str());
            return false;
        }
    }
}

void FilterController::release() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (eglCore != nullptr) {
        eglCore->release();
    }
    if (!filterList.empty()) {
        for (IFilter* filter: filterList) {
            filter->releaseTexture();
        }
    }
    filterList.clear();
    listener = nullptr;
}

void FilterController::initPixelBuffer() {
    destroyPixelBuffers();
    pixelBuffer = -1;

    if (option != nullptr) {
        if (option->getScaleRatio() != 1) {
            scaleImgWidth = option->srcWidth * option->getScaleRatio();
            scaleImgHeight = option->srcHeight * option->getScaleRatio();
        } else if (option->getScaleWidth() != -1 && option->getScaleHeight() != -1) {
            scaleImgWidth = option->getScaleWidth();
            scaleImgHeight = option->getScaleHeight();
        }
    }

    int align = 128;//128字节对齐
    imgSize = ((scaleImgWidth * 4 + (align - 1)) & ~(align - 1))* scaleImgHeight;
//    mPhoSize = width*height*4;

    glGenBuffers(1,&pixelBuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
    glBufferData(GL_PIXEL_PACK_BUFFER, imgSize, nullptr, GL_STATIC_READ);
}


void FilterController::drawPixelBuffer() {
    if (pixelBuffer < 0) {
        HLOGE("pixelBuffer not init");
        return;
    }
    if (scaleImgWidth > 0 && scaleImgHeight > 0) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
        glReadPixels(0, 0, scaleImgWidth, scaleImgHeight, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        if (imgSize > 0) {
            HLOGV("imgSize = %",  imgSize);
            saveImgData = (unsigned char *) glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, imgSize,
                                                             GL_MAP_READ_BIT);
        } else {
            HLOGE("imgSize = 0");
        }
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        //解除绑定PBO
        glBindBuffer(GL_PIXEL_PACK_BUFFER, GL_NONE);
    } else {
        HLOGE("scaleImageWith or scaleImageHeight = 0");
        return;
    }
}

void FilterController::destroyPixelBuffers() {
    if (pixelBuffer > 0) {
        glDeleteTextures(1, &pixelBuffer);
    }
}


void FilterController::setListener(FilterListener *listener) {
    this->listener = listener;
}
