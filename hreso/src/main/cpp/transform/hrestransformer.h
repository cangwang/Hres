//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_HRESTRANSFORMER_H
#define HRES_HRESTRANSFORMER_H

#include <android/log.h>
#include <string>

#define LOG_TAG "HresTransformer"
#define ELOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ELOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class HresTransformer {
public:
    HresTransformer();
    ~HresTransformer();
    void setOption(string options);
    void transform();
    void release();

private:

};


#endif //HRES_HRESTRANSFORMER_H
