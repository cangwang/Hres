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
        optionsList.push_front(option);
    } else {
        optionsList.push_back(option);
    }
}

void HresTransformer::transform() {
    if (eglCore == nullptr) {
        eglCore = new EGLCore();
        eglCore->start();
    }
    if (!optionsList.empty()) {
        auto options = optionsList.front();
        optionsList.pop_front();
        if (options->getType() == 1) {
            if (imageHresTransformer == nullptr) {
                imageHresTransformer = new ImageHresTransformer();
            }
            imageHresTransformer->transformOption(options);
        }
    }
}

void HresTransformer::release() {
    if (eglCore) {
        eglCore->release();
        eglCore = nullptr;
    }
    if (imageHresTransformer != nullptr) {
        imageHresTransformer->release();
        imageHresTransformer = nullptr;
    }
}

bool HresTransformer::removeOptions(string address) {
    for (auto i = optionsList.begin(); i < optionsList.end(); i++) {
        if (i.operator*()->getAddress() == address) {
            optionsList.erase(i);
            return true;
        }
    }

    return false;
}
