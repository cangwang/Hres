//
// Created by cangwang on 2022/10/14.
//

#include "hrestransformer.h"

HresTransformer::HresTransformer(): optionParser(nullptr) {
    optionsList = make_shared<deque<IOptions*>>();
}

HresTransformer::~HresTransformer() {
    optionsList->clear();
    if (optionParser != nullptr) {
        optionParser = nullptr;
    }
    listenerManager = nullptr;
}

void HresTransformer::setListener(ListenerManager* listenerManager) {
   this->listenerManager = listenerManager;
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
