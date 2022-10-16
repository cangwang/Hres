//
// Created by asus on 2022/10/15.
//
#include <string>
#include <util/parson.h>
#ifndef HRES_IOPTIONS_H
#define HRES_IOPTIONS_H

using namespace std;
class IOptions {
public:
    virtual void setOptions(string options) = 0;
    virtual string getName() = 0;
    virtual int getType() = 0;  // 1是图片 2是视频
    virtual string getFilterType() = 0;  // 滤镜类型
    virtual string getAddress() = 0;
    virtual int getScaleType() = 0; //1 使用scaleRatio, 2使用固定宽高
    virtual float getScaleRatio() = 0;
    virtual int getScaleWidth() = 0;
    virtual int getScaleHeight() = 0;
    virtual bool getAsync() = 0;

protected:
    string name;
    int type;
    string filterType;
    string address;
    int scaleType;
    float scaleRatio;
    int scaleWidth;
    int scaleHeight;
    bool async;
};

#endif //HRES_IOPTIONS_H


