//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_IMAGEOPTIONPARAMS_H
#define HRES_IMAGEOPTIONPARAMS_H

#include <transform/ioptions.h>

#define LOG_TAG "ImageOptionParams"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class ImageOptionParams : public IOptions {
public:
    string getName() override;

    int getType() override;

    string getAddress() override;

    string getSaveAddress() override;

    int getScaleType() override;

    float getScaleRatio() override;

    int getScaleWidth() override;

    int getScaleHeight() override;

    bool getAsync() override;

    string getFilterType() override;

    bool getFront() override;

    void setOptions(string options) override;

};


#endif //HRES_IMAGEOPTIONPARAMS_H
