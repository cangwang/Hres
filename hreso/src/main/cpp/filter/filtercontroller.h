//
// Created by cangwang on 2022/10/22.
//

#ifndef HRES_FILTERCONTROLLER_H
#define HRES_FILTERCONTROLLER_H

#include <transform/ioptions.h>
#include <filter/ifilter.h>
#include <filter/simplefilter.h>
#include <filter/fsrfilter.h>
#include <list>
#include <egl/eglcore.h>
#include <util/stb_image_write.h>
#include <filter/filterlistener.h>
#include <util/loadtextureutil.h>
#include <stdio.h>
#include <mutex>
#include <util/threadpool.h>
#include <thread>
#include "fbfilter.h"

#define LOG_TAG "FilterController"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class FilterController {
public:
    FilterController();
    ~FilterController();
    void setWindow(ANativeWindow* window);
    void updateViewPoint(int width, int height);
    void transformFilter(IOptions* option);
    void transformFilterInThread(IOptions* option);
    void setListener(FilterListener* listener);
    bool render();
    void renderInThread();
    void saveInThread();
    void release();
private:
    mutex gMutex;
    shared_ptr<EGLCore> eglCore;
    IOptions* option;
    list<IFilter*> filterList;
    string filterName;
    FilterListener* listener;
    ThreadPool* pool;
    ANativeWindow* window;
    int surfaceWidth = 0;
    int surfaceHeight = 0;

    shared_ptr<FbFilter> fbFilter;
};


#endif //HRES_FILTERCONTROLLER_H
