//
// Created by zengjiale on 2022/10/14.
//

#include "imagehrestransformer.h"

ImageHresTransformer::ImageHresTransformer() {

}

ImageHresTransformer::~ImageHresTransformer() {
    filterList.clear();
}

void ImageHresTransformer::transformOption(IOptions *option) {
    if (this->option.get() == nullptr
        || (this->option.get() != nullptr && this->option->getName() != option->getName())) {  //滤镜模式不一样
        if (option->getName() == "simple") {
            filterList.push_front(new SimpleFilter());
        }
    }

    this->option = shared_ptr<IOptions>(option);

}

void ImageHresTransformer::transform() {

}

void ImageHresTransformer::release() {
    filterList.clear();
}
