//
// Created by asus on 2022/10/16.
//

#ifndef HRES_OPTIONPARSER_H
#define HRES_OPTIONPARSER_H

#include <transform/ioptions.h>
#include <android/log.h>
#include <string>

#define LOG_TAG "OptionParser"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

class OptionParser {
public:
    IOptions* parseOptions(string options);
};


#endif //HRES_OPTIONPARSER_H
