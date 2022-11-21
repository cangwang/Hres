//
// Created by asus on 2022/11/18.
//

#ifndef HRES_HRESBASETRANSFORMER_H
#define HRES_HRESBASETRANSFORMER_H

#include <src/main/cpp/bean/ioptions.h>
#include <filter/filterlistener.h>
#include <android/native_window.h>

class HresBaseTransformer {
public:
    virtual void setWindow(ANativeWindow* window) = 0;
    virtual void updateViewPoint(int width, int height) = 0;
    virtual void transformOption(IOptions* options) = 0;
    virtual void setListener(FilterListener* listener) = 0;
    virtual void transform() = 0;
    virtual void release() = 0;
};

#endif //HRES_HRESBASETRANSFORMER_H
