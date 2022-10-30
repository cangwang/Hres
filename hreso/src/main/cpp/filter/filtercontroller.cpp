//
// Created by cangwang on 2022/10/22.
//

#include <unistd.h>
#include "filtercontroller.h"



FilterController::FilterController(): pool(new ThreadPool(1)) {
    pool->init();
}

FilterController::~FilterController() {
    filterList.clear();
    destroyPixelBuffers();
    option = nullptr;
    if (saveImgData) {
        memset(saveImgData, 0, imgSize);
        saveImgData = nullptr;
    }
    listener = nullptr;

    if (eglCore) {
        eglCore->release();
    }
    if (saveImgData) {
        saveImgData = nullptr;
    }
    if (pool) {
        pool->shutdown();
        pool = nullptr;
    }
}

void FilterController::setWindow(ANativeWindow *window) {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start(window);
    }
//
//    unique_lock<mutex> lock(gMutex);
//    auto startWindow = [&](ANativeWindow *window) -> void
//    {
//        if (eglCore == nullptr) {
//            eglCore = make_shared<EGLCore>();
//            eglCore->start(window);
//        }
//    };
//    if (pool != nullptr) {
//        pool->submit(startWindow, window);
//    }
}

void FilterController::transformFilter(IOptions *option) {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start(nullptr);
    }
    if (option == nullptr) {
        HLOGV("transformFilter option nullptr");
        return;
    }
    if (filterName != option->getFilterType()) {  //滤镜模式不一样
        if (option->getFilterType() == "simple") {
            IFilter* filter = new SimpleFilter();
            LoadTextureUtil::loadTextureFromOption(option);
            filter->setOptions(option);
            filterList.push_front(filter);
        }
        filterName = option->getFilterType();
    }

    this->option = option;
}

void FilterController::transformFilterInThread(IOptions *option) {
    transformFilter(option);

//    //加锁
//    unique_lock<mutex> lock(gMutex);
//    auto transformFilterWork = [&](IOptions* o) -> void
//    {
//        transformFilter(o);
//    };
//    if (pool != nullptr) {
//        pool->submit(transformFilterWork, option);
//    }
}

void FilterController::render() {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start(nullptr);
    }
//    glClearColor(0, 0, 0, 0);
//    glClear(GL_COLOR_BUFFER_BIT);
//    initPixelBuffer();
    if (!filterList.empty()) {
        for (IFilter* filter: filterList) {
            filter->renderFrame();
        }
    } else {
        HLOGE("filterList is empty");
        if (listener && option) {
            listener->filterRenderFail(option, string("filterList is empty"));
        }
//        return "filterList is empty";
    }

    if (eglCore != nullptr) {
        eglCore->swapBuffer();
    }
//    drawPixelBuffer();
//    readBuffer();
    if (option != nullptr) {  //保存图片
        return save(option);
    }
//    return "option is null";
}

void FilterController::renderInThread() {
    render();
//    //加锁
////    unique_lock<mutex> lock(gMutex);
//    auto renderWork = [&]() -> void
//    {
//        render();
//    };
//    if (pool != nullptr) {
//        pool->submit(renderWork);
//    }
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
//        if (result) {
//            return nullptr;
//        } else {
//            return "saveImage fail";
//        }
    } else {
        if (listener != nullptr) {
            listener->filterRenderFail(option, string("scaleImgWidth || scaleImgHeight = 0"));
        }
//        return "scaleImgWidth || scaleImgHeight = 0";
    }
}

void FilterController::readBuffer() {
    //加锁
    std::unique_lock<std::mutex> lock(gMutex);

    long size = scaleImgWidth * scaleImgHeight * 4;
    saveImgData = (unsigned char *) malloc(sizeof(unsigned char)*size);
    //清空数据
    memset(saveImgData,0, sizeof(unsigned char)*size);

//    glReadBuffer(GL_FRONT);
    //对齐像素字节
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    checkGLError("glPixelStorei");
    //获取帧内字节
    glReadPixels(0, 0, scaleImgWidth, scaleImgHeight, GL_RGBA, GL_UNSIGNED_BYTE, saveImgData);
}


bool FilterController::saveImg(const string saveFileAddress,unsigned char* data,int width,int height, int channel) {
//    FILE *file = fopen(saveFileAddress.c_str(), "wb+");
//    if (file == nullptr) {
//        return false;
//    }

    //屏幕到文件保存需要使用
    stbi_flip_vertically_on_write(1);
    //保存图片到本地文件
//    if (channel == 3) {
//        if (stbi_write_jpg(saveFileAddress.c_str(), width, height, channel, data, 0)) {
//            HLOGV("save address = %s success", saveFileAddress.c_str());
////            free(data);
//            memset(saveImgData, 0, imgSize);
//            return true;
//        } else {
////            free(data);
//            memset(saveImgData, 0, imgSize);
//            HLOGE("save fail address = %s fail", saveFileAddress.c_str());
//            return false;
//        }
//    } else if (channel == 4) {
        if (stbi_write_png(saveFileAddress.c_str(), width, height, 4, data, 0)) {
            HLOGV("save address = %s success", saveFileAddress.c_str());
//            free(data);
            memset(saveImgData, 0, imgSize);
            return true;
        } else {
//            free(data);
            memset(saveImgData, 0, imgSize);
            HLOGE("save fail address = %s fail", saveFileAddress.c_str());
            return false;
        }
//    }

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

    destroyPixelBuffers();
    option = nullptr;
    if (saveImgData) {
        memset(saveImgData, 0, imgSize);
        saveImgData = nullptr;
    }
}

void FilterController::initPixelBuffer() {
    destroyPixelBuffers();
    HLOGV("initPixelBuffer");
    pixelBuffer = -1;

    if (option != nullptr) {
        if (option->getScaleRatio() != 1) {
            scaleImgWidth = option->srcWidth * option->getScaleRatio();
            scaleImgHeight = option->srcHeight * option->getScaleRatio();
        } else if (option->getScaleWidth() > 0 && option->getScaleHeight() > 0) {
            scaleImgWidth = option->getScaleWidth();
            scaleImgHeight = option->getScaleHeight();
        } else {
            scaleImgWidth = option->srcWidth;
            scaleImgHeight = option->srcHeight;
        }
    }

    int align = 128;//128字节对齐
//    imgSize = ((scaleImgWidth * 4 + (align - 1)) & ~(align - 1))* scaleImgHeight;
    imgSize = scaleImgWidth * scaleImgHeight * 4;

    glGenBuffers(1,&pixelBuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
    glBufferData(GL_PIXEL_PACK_BUFFER, imgSize, nullptr, GL_STATIC_READ);
}


void FilterController::drawPixelBuffer() {
    HLOGV("drawPixelBuffer");
    if (pixelBuffer < 0) {
        HLOGE("pixelBuffer not init");
        return;
    }
    if (scaleImgWidth > 0 && scaleImgHeight > 0) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
        glReadPixels(0, 0, scaleImgWidth, scaleImgHeight, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        if (imgSize > 0) {
            HLOGV("imgSize = %ld",  imgSize);
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
    HLOGV("destroyPixelBuffers");
    if (pixelBuffer > 0) {
        glDeleteTextures(1, &pixelBuffer);
    }
}


void FilterController::setListener(FilterListener *listener) {
    this->listener = listener;
}

//检测错误
void FilterController::checkGLError(std::string op) {
    GLint error = glGetError();
    if(error != GL_NO_ERROR){
        char err = (char)error;
        HLOGE("%s :glError 0x%d",op.c_str(),err);
    }
}
