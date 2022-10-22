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
        HLOGV("saveAddress is null, use orgin address");
        address = option->getAddress();
    } else {
        address = option->getSaveAddress();
    }
}

void FilterController::release() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (eglCore != nullptr) {
        eglCore->release();
    }
    filterList.clear();
}
