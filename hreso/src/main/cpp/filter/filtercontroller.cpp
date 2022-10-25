//
// Created by asus on 2022/10/22.
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
    this->option = option;
}

void FilterController::render() {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start();
    }
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!filterList.empty()) {
        for (IFilter* filter: filterList) {
            filter->renderFrame();
        }
    }

    if (eglCore != nullptr) {
        eglCore->swapBuffer();
    }
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
}

bool FilterController::saveImg(string saveFileAddress,unsigned char* data,int width,int height,int type) {
    //屏幕到文件保存需要使用
    stbi_flip_vertically_on_write(1);
    //保存图片到本地文件
    if (stbi_write_png(saveFileAddress.c_str(), width, height, 4, data, 0)) {
        HLOGV("save address = %s success", saveFileAddress.c_str());
        if(type ==1)
            free(data);
        return true;
    } else {
        if(type ==1)
            free(data);
        HLOGE("save address = %s fail", saveFileAddress.c_str());
        return false;
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
}

void FilterController::initPixelBuffer() {
    destroyPixelBuffers();
    pixelBuffer = 0;

    if (option != nullptr) {
        if (option->getScaleRatio() != 1) {

        } else if (option->getScaleWidth() != -1 && option->getScaleHeight() != -1) {

        }
    }

    int align = 128;//128字节对齐
    imgSize = ((scaleImgWidth * 4 + (align - 1)) & ~(align - 1))* scaleImgHeight;
//    mPhoSize = width*height*4;

    glGenBuffers(1,&pixelBuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
    glBufferData(GL_PIXEL_PACK_BUFFER,imgSize, nullptr,GL_STATIC_READ);
}


void FilterController::drawPixelBuffer() {
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
    }
}

void FilterController::destroyPixelBuffers() {
    if (pixelBuffer > 0) {
        glDeleteTextures(1, &pixelBuffer);
    }
}

