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
//    filterList.
}

void ImageHresTransformer::release() {
    filterList.clear();
}
