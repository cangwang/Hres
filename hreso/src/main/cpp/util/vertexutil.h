//
// Created by zengjiale on 2022/4/18.
//



#ifndef YYEVA_VETEXUTIL_HPP
#define YYEVA_VETEXUTIL_HPP

#include <android/log.h>
#include <bean/pointrect.h>
#define LOG_TAG "VertexUtil"
#define ELOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class VertexUtil {
public:
    static float* create(int width, int height, PointRect* rect, float* array);
    static float* create(int width, int height, PointRect* rect, float *array,float ratio, float sizeRatio);
    static float switchX(float x);
    //加入ratio参数，计算图像正常比例
    static float* create(int width, int height, PointRect* rect, float* array, float ratio);
    //加入ratio参数，计算图像正常比例
    static float switchX(float x, float ratio);
    static float switchY(float y);
};


#endif //YYEVA_VETEXUTIL_HPP
