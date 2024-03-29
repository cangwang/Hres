//
// Created by cangwang on 2022/10/14.
//

#ifndef HRES_IMAGEOPTIONPARAMS_H
#define HRES_IMAGEOPTIONPARAMS_H

#include <transform/ioptions.h>
#include <android/log.h>

#define LOG_TAG "ImageOptionParams"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class ImageOptionParams : public IOptions {
public:
    void setOptions(string options, jobject op) override;
};


#endif //HRES_IMAGEOPTIONPARAMS_H
