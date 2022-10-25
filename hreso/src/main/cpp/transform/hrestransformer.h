//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_HRESTRANSFORMER_H
#define HRES_HRESTRANSFORMER_H

#include <android/log.h>
#include <string>
#include <deque>
#include <transform/ioptions.h>
#include <transform/imagehrestransformer.h>
#include <egl/eglcore.h>
#include <transform/optionparser.h>
#include <jni.h>

#define LOG_TAG "HresTransformer"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class HresTransformer {
public:
    HresTransformer();
    ~HresTransformer();
    void addOption(string options);
    void transform();
    void release();
    void setManager(jobject listener);

private:
    shared_ptr<OptionParser> optionParser;
    shared_ptr<deque<IOptions*>> optionsList;
    shared_ptr<ImageHresTransformer> imageHresTransformer;
    shared_ptr<>
    bool removeOptions(string address);
};


#endif //HRES_HRESTRANSFORMER_H
