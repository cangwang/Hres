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
    virtual void setOptions(string options) {
        this->options = options;
    };
    virtual string getName() {
        return name;
    };
    virtual int getType() {
        return type;
    };  // 1是图片 2是视频
    virtual string getFilterType() {
        return filterType;
    };  // 滤镜类型
    virtual string getAddress() {
        return address;
    };
    virtual string getSaveAddress() {
        return saveAddress;
    };
    virtual int getScaleType() {
        return scaleType;
    }; //1 使用scaleRatio, 2使用固定宽高
    virtual float getScaleRatio() {
        return scaleRatio;
    };
    virtual int getScaleWidth() {
        return scaleWidth;
    };
    virtual int getScaleHeight() {
        return scaleHeight;
    };
    virtual bool getFront() {
        return front;
    };
    virtual bool getAsync() {
        return async;
    };
    virtual string getString() {
        return options;
    };

protected:
    string name;
    int type;
    string filterType;
    string address;
    string saveAddress;
    int scaleType;
    float scaleRatio;
    int scaleWidth;
    int scaleHeight;
    bool front;  // 是否插入队头
    bool async;
    string options;
};

#endif //HRES_IOPTIONS_H


