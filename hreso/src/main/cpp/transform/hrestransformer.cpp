//
// Created by zengjiale on 2022/10/14.
//

#include "hrestransformer.h"

HresTransformer::HresTransformer():eglCore(new EGLCore()),
                                    imageHresTransformer(nullptr),
                                    optionParser(nullptr) {
    eglCore->start();
    optionsList.clear();
}

HresTransformer::~HresTransformer() {
    optionsList.clear();
    if (optionParser != nullptr) {
        optionParser = nullptr;
    }
}

void HresTransformer::addOption(string options) {
    if (optionParser == nullptr) {
        optionParser = new OptionParser();
    }
    IOptions* option = optionParser->parseOptions(options);
    if (option->getFront()) {
        optionsList.push_front(option)
    } else {
        optionsList.push_back(option);
    }
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
