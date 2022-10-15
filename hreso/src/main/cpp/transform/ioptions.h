//
// Created by asus on 2022/10/15.
//
#include <string>
#ifndef HRES_IOPTIONS_H
#define HRES_IOPTIONS_H

using namespace std;
class IOptions {
public:
    virtual string getName() = 0;
    virtual int getType() = 0;  // 1是图片 2是视频
    virtual string getAddress() = 0;
    virtual int getScaleType() = 0; //1 使用scaleRatio, 2使用固定宽高
    virtual float getScaleRatio() = 0;
    virtual float getScaleWidth() = 0;
    virtual float getScaleHeight() = 0;
    virtual bool getAsync() = 0;
};

#endif //HRES_IOPTIONS_H


