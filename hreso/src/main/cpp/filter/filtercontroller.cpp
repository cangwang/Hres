//
// Created by cangwang on 2022/10/22.
//

#include <unistd.h>
#include "filtercontroller.h"

FilterController::FilterController(): pool(new ThreadPool(1)), window(nullptr) {
    pool->init();
}

FilterController::~FilterController() {
    release();
    if (pool) {
        pool->shutdown();
        pool = nullptr;
    }
}

//显示屏幕加载
void FilterController::setWindow(ANativeWindow *window) {
//    if (eglCore == nullptr) {
//        eglCore = make_shared<EGLCore>();
//        eglCore->start(window);
//    }
    this->window = window;
    unique_lock<mutex> lock(gMutex);
    auto startWindow = [&](ANativeWindow *window) -> void
    {
        if (eglCore == nullptr) {
            eglCore = make_shared<EGLCore>();
            eglCore->start(window, 0, 0);
        }
    };
    if (pool != nullptr) {
        pool->submit(startWindow, window);
    }
}

void FilterController::updateViewPoint(int width, int height) {
    this->surfaceWidth = width;
    this->surfaceHeight = height;
}

void FilterController::transformFilter(IOptions *option) {
    if (option == nullptr) {
        HLOGV("transformFilter option nullptr");
        return;
    }
    if (eglCore == nullptr) { //后台Surface转换
        eglCore = make_shared<EGLCore>();
        int width = 0;
        int height = 0;
        int channel = 0;
        LoadTextureUtil::loadWidthHeightFromOption(option, &width, &height, &channel);
        eglCore->start(nullptr, width, height);
    }

    //读取textureId到option
    LoadTextureUtil::loadTextureFromOption(option);
    if (filterName != option->getFilterType()) {  //滤镜模式不一样
        if (option->getFilterType() == "simple") {
            IFilter* filter = new SimpleFilter();
            filter->setOptions(option);
            filterList.push_front(filter);
        }
        filterName = option->getFilterType();
    }
    if (fbFilter == nullptr) {
        fbFilter = make_shared<FbFilter>();
        fbFilter->setListener(listener);
        fbFilter->setOptions(option);
    }

    this->option = option;
}

void FilterController::transformFilterInThread(IOptions *option) {
//    transformFilter(option);

    //加锁
    unique_lock<mutex> lock(gMutex);
    auto transformFilterWork = [&](IOptions* o) -> void
    {
        transformFilter(o);
    };
    if (pool != nullptr) {
        pool->submit(transformFilterWork, option);
    }
}

void FilterController::render() {
    //设置视屏大小
    if (option != nullptr) {
        glViewport(0, 0, option->getScaleWidth(), option->getScaleHeight());
    } else {
        HLOGE("option is null, no glViewport");
    }

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!filterList.empty()) {
        if (fbFilter != nullptr) {
            fbFilter->enableFramebuffer();
        }
        for (IFilter* filter: filterList) {
            filter->renderFrame();
        }
        if (fbFilter != nullptr) {
            fbFilter->disableFrameBuffer();
        }
    } else {
        HLOGE("filterList is empty");
        if (listener && option) {
            listener->filterRenderFail(option, string("filterList is empty"));
        }
//        return "filterList is empty";
    }
    if (fbFilter != nullptr) {
        fbFilter->renderFrame();
    }

    glFlush();
    if (eglCore != nullptr) {
        eglCore->swapBuffer();
    }
//    return "option is null";
}

void FilterController::renderInThread() {
//    render();
    //加锁
    unique_lock<mutex> lock(gMutex);
    auto renderWork = [&]() -> void
    {
        render();
    };
    if (pool != nullptr) {
        pool->submit(renderWork);
    }
}

void FilterController::release() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!filterList.empty()) {
        for (IFilter* filter: filterList) {
            filter->releaseTexture();
        }
    }
    filterList.clear();
    listener = nullptr;

    if (eglCore != nullptr) {
        eglCore->release();
        eglCore = nullptr;
    }
}

void FilterController::setListener(FilterListener *listener) {
    this->listener = listener;
}
