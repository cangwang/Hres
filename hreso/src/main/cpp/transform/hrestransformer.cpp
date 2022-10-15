//
// Created by zengjiale on 2022/10/14.
//

#include "hrestransformer.h"

HresTransformer::HresTransformer():eglCore(new EGLCore()), imageHresTransformer(nullptr) {
    eglCore->start();
}

HresTransformer::~HresTransformer() {
    optionsList.clear();
}

void HresTransformer::addOption(string options) {

}

void HresTransformer::transform() {
    if (eglCore == nullptr) {
        eglCore = new EGLCore();
        eglCore->start();
    }
}

void HresTransformer::release() {
    if (eglCore) {
        eglCore->release();
        eglCore = nullptr;
    }
}

bool HresTransformer::removeOptions(string address) {
    for(IOptions* options: optionsList) {
        if (options->getAddress() == address) {
            optionsList.remove(options);
            return true;
        }
    }
    return false;
}
