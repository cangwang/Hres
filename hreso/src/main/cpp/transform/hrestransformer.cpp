//
// Created by zengjiale on 2022/10/14.
//

#include "hrestransformer.h"

HresTransformer::HresTransformer():eglCore(new EGLCore()),
                                    optionParser(nullptr) {
    eglCore->start();
    optionsList = make_shared<deque<IOptions*>>();
}

HresTransformer::~HresTransformer() {
    optionsList->clear();
    if (optionParser != nullptr) {
        optionParser = nullptr;
    }
}

void HresTransformer::addOption(string options) {
    if (optionParser == nullptr) {
        optionParser = shared_ptr<OptionParser>();
    }
    IOptions* option = optionParser->parseOptions(options);
    if (option->getFront()) {
        optionsList->push_front(option);
    } else {
        optionsList->push_back(option);
    }
}

void HresTransformer::transform() {
    if (eglCore == nullptr) {
        eglCore = make_shared<EGLCore>();
        eglCore->start();
    }
    if (!optionsList->empty()) {
        auto options = optionsList->front();
        optionsList->pop_front();
        if (options->getType() == 1) {  //类型为图片
            if (imageHresTransformer == nullptr) {
                imageHresTransformer = make_shared<ImageHresTransformer>();
            }
            imageHresTransformer->transformOption(options);
        }
    }
}

void HresTransformer::release() {
    optionsList->clear();
    if (eglCore) {
        eglCore->release();
    }
    if (imageHresTransformer != nullptr) {
        imageHresTransformer->release();
    }
}

bool HresTransformer::removeOptions(string address) {
    for (auto i = optionsList->begin(); i < optionsList->end(); i++) {
        if (i.operator*()->getAddress() == address) {
            optionsList->erase(i);
            return true;
        }
    }
    return false;
}
