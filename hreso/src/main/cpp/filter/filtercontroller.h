//
// Created by asus on 2022/10/22.
//

#ifndef HRES_FILTERCONTROLLER_H
#define HRES_FILTERCONTROLLER_H

#include <transform/ioptions.h>
#include <filter/ifilter.h>
#include <filter/simplefilter.h>
#include <list>
#include <egl/eglcore.h>

#define LOG_TAG "FilterController"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class FilterController {
public:
    FilterController();
    ~FilterController();
    void transformFilter(IOptions* option);
    void save(IOptions* option);
    void render();
    void release();
private:
    shared_ptr<EGLCore> eglCore;
    IOptions* option;
    list<IFilter*> filterList;
    string filterName;
};


#endif //HRES_FILTERCONTROLLER_H
