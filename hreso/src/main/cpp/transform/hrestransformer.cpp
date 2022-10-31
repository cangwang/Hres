//
// Created by cangwang on 2022/10/14.
//

#include "hrestransformer.h"

HresTransformer::HresTransformer(): optionParser(nullptr) {
    optionsList = make_shared<deque<IOptions*>>();
}

HresTransformer::~HresTransformer() {
    release();
}

void HresTransformer::setWindow(ANativeWindow *window) {
    if (imageHresTransformer == nullptr) {
        imageHresTransformer = make_shared<ImageHresTransformer>();
    }
    imageHresTransformer->setWindow(window);
}

void HresTransformer::updateViewPoint(int width, int height) {
    if (imageHresTransformer != nullptr) {
        imageHresTransformer->updateViewPoint(width, height);
    }
}

void HresTransformer::setListener(ListenerManager* listenerManager) {
   this->listenerManager = listenerManager;
}

void HresTransformer::addOption(string options, jobject op) {
    if (optionParser == nullptr) {
        optionParser = shared_ptr<OptionParser>();
    }
    IOptions* option = optionParser->parseOptions(options, op);
    if (option->getFront()) {
        optionsList->push_front(option);
    } else {
        optionsList->push_back(option);
    }
    if (!isTransforming) {
        transform();
    }
}

void HresTransformer::transform() {
    if (!optionsList->empty()) {
        isTransforming = true;
        auto options = optionsList->front();
        optionsList->pop_front();
        if (options != nullptr) {
            if (listenerManager != nullptr) {
                listenerManager->hresTransformStart(options->getObj());
            }
            if (options->getType() == 1) {  //类型为图片
                if (imageHresTransformer == nullptr) {
                    imageHresTransformer = make_shared<ImageHresTransformer>();
                }
                imageHresTransformer->setListener(this);
                imageHresTransformer->transformOption(options);
                imageHresTransformer->transform();
            }
        }
    } else {
        isTransforming = false;
    }
}

void HresTransformer::release() {
    optionsList->clear();
    if (optionParser != nullptr) {
        optionParser = nullptr;
    }
    if (imageHresTransformer != nullptr) {
        imageHresTransformer->release();
        imageHresTransformer = nullptr;
    }
    if (listenerManager != nullptr) {
        listenerManager = nullptr;
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

void HresTransformer::filterRenderComplete(IOptions *options) {
    if (listenerManager != nullptr) {
        listenerManager->hresTransformComplete(options->getObj());
    }
    //抽取下一个进行转换
    transform();
}

void HresTransformer::filterRenderFail(IOptions *options, string errorMsg) {
    if (listenerManager != nullptr) {
        listenerManager->hresTransformError(options->getObj(), errorMsg);
    }
}
