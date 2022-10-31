//
// Created by cangwang on 2022/10/14.
//

#include "imagehrestransformer.h"

ImageHresTransformer::ImageHresTransformer(): filterController(make_shared<FilterController>()) {

}

ImageHresTransformer::~ImageHresTransformer() {
    release();
}

void ImageHresTransformer::setWindow(ANativeWindow *window) {
    if (filterController != nullptr) {
        filterController->setWindow(window);
    }
}

void ImageHresTransformer::updateViewPoint(int width, int height) {
    if (filterController != nullptr) {
        filterController->updateViewPoint(width, height);
    }
}

void ImageHresTransformer::setListener(FilterListener *listener) {
    if (filterController != nullptr) {
        filterController->setListener(listener);
    }
}

void ImageHresTransformer::transformOption(IOptions *option) {
    if (filterController != nullptr) {
//        filterController->transformFilter(option);
        filterController->transformFilterInThread(option);
    }
}

void ImageHresTransformer::transform() {
    if (filterController != nullptr) {
//        filterController->render();
        filterController->renderInThread();
    }
}

void ImageHresTransformer::release() {
    if (filterController != nullptr) {
        filterController->release();
    }
}
