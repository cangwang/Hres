//
// Created by cangwang on 2022/10/14.
//

#include "imagehrestransformer.h"

ImageHresTransformer::ImageHresTransformer(): filterController(make_shared<FilterController>()) {

}

ImageHresTransformer::~ImageHresTransformer() {

}

void ImageHresTransformer::transformOption(IOptions *option) {
    if (filterController != nullptr) {
        filterController->transformFilter(option);
    }
}

void ImageHresTransformer::transform() {
    if (filterController != nullptr) {
        filterController->render();
    }
}

void ImageHresTransformer::release() {
    filterController->release();
}
