//
// Created by cangwang on 2022/10/20.
//

#ifndef HRES_LOADTEXTUREUTIL_H
#define HRES_LOADTEXTUREUTIL_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <util/stb_image.h>
#include <android/log.h>
#include <transform/ioptions.h>

#define LOG_TAG "LoadTextureUtil"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class LoadTextureUtil {
public:
    static GLuint loadTextureFromFile(const char *fileName, int *w, int *h,int *n);
    static GLuint loadTextureFromOption(IOptions* options);
    static void loadWidthHeightFromOption(IOptions* options);
};


#endif //HRES_LOADTEXTUREUTIL_H
