//
// Created by cangwang on 2022/10/22.
//

#ifndef HRES_FILTERCONTROLLER_H
#define HRES_FILTERCONTROLLER_H

#include <transform/ioptions.h>
#include <filter/ifilter.h>
#include <filter/simplefilter.h>
#include <list>
#include <egl/eglcore.h>
#include <util/stb_image_write.h>
#include <transform/listenermanager.h>
#include <util/loadtextureutil.h>

#define LOG_TAG "FilterController"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class FilterController {
public:
    FilterController();
    ~FilterController();
    void transformFilter(IOptions* option);
    void setListenManager(ListenerManager* listenerManager);
    void save(IOptions* option);
    void render();
    void release();
private:
    shared_ptr<EGLCore> eglCore;
    IOptions* option;
    list<IFilter*> filterList;
    string filterName;
    GLuint pixelBuffer;
    unsigned char* saveImgData;
    long imgSize;
    int imgWidth = -1;
    int imgHeight = -1;
    int scaleImgWidth = -1;
    int scaleImgHeight = -1;
    ListenerManager* listenerManager;

    void initPixelBuffer();
    void drawPixelBuffer();
    void destroyPixelBuffers();
    bool saveImg(const string& saveFileAddress,unsigned char* data,int width,int height,int type);
};


#endif //HRES_FILTERCONTROLLER_H
