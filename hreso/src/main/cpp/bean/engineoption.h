//
// Created by asus on 2022/11/18.
//

#ifndef HRES_ENGINEOPTION_H
#define HRES_ENGINEOPTION_H

#include <android/log.h>
#include <string>
#include <util/parson.h>

#define LOG_TAG "EngineOption"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class EngineOption {
public:
    void setOptions(string options);

    bool useVulkan;
    string transformType;
};


#endif //HRES_ENGINEOPTION_H
