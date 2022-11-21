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

void HresTransformer::setEngineOption(string option) {
    if (!option.empty()) {
        engineOption = new EngineOption();
        engineOption->setOptions(option);
        if (engineOption->transformType == "img") {
            if (engineOption->useVulkan) {
                transformer = make_shared<ImageVulkanHresTransformer>();
            } else {
                transformer = make_shared<ImageHresTransformer>();
            }
        }
    } else {
        HLOGE("setEngineOption, option is null");
    }
}

void HresTransformer::setWindow(ANativeWindow *window) {
    if (transformer != nullptr) {
        transformer->setWindow(window);
    } else {
        HLOGE("setWindow, please setEngineOption first");
    }
}

void HresTransformer::updateViewPoint(int width, int height) {
    if (transformer != nullptr) {
        transformer->updateViewPoint(width, height);
    } else {
        HLOGE("updateViewPoint,please setEngineOption first");
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
                if (transformer != nullptr) {
                    transformer->setListener(this);
                    transformer->transformOption(options);
                    transformer->transform();
                } else {
                    HLOGE("updateViewPoint,please setEngineOption first");
                }
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
    if (transformer != nullptr) {
        transformer->release();
        transformer = nullptr;
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
    //抽取下一个进行转换
    transform();
}
