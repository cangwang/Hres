//
// Created by cangwang on 2022/10/15.
//
#include <string>
#include <util/parson.h>
#include <jni.h>
#ifndef HRES_IOPTIONS_H
#define HRES_IOPTIONS_H

using namespace std;
class IOptions {
public:
    ~IOptions() {
        options.clear();
        op = nullptr;
    }
    virtual void setOptions(string options, jobject op) {
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

    virtual jobject getObj() {
        return op;
    }

    int srcWidth;  //原图宽度
    int srcHeight;  //圆度高度
    int srcChannel; //原图通道数
    int textureId; //纹理id

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
    jobject op; // option java圆形
};

#endif //HRES_IOPTIONS_H


