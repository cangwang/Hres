//
// Created by cangwang on 2022/10/20.
//

#include "loadtextureutil.h"

GLuint LoadTextureUtil::loadTextureFromFile(const char *fileName, int *w, int *h,int *n) {
    GLuint textureHandler=0;
    glGenTextures(1,&textureHandler);

    if (textureHandler != GL_NONE){
        glBindTexture(GL_TEXTURE_2D,textureHandler);
        //纹理放大缩小使用线性插值
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //超出的部份会重复纹理坐标的边缘，产生一种边缘被拉伸的效果，s/t相当于x/y轴坐标
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        //读取图片长宽高数据
        unsigned char* data = stbi_load(fileName, w, h, n, 0);

        HLOGV("loadTexture fileName = %s,width = %d,height=%d,n=%d",fileName,*w,*h,*n);

        if(data != nullptr) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *w, *h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            stbi_image_free(data);
            return textureHandler;
        } else {
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            HLOGE("load texture is null,fileName = %s",fileName);
            stbi_image_free(data);

            return 0; //代表加载图片失败
        }
    } else  {
        HLOGE("glGenTextures 0 ,maybe not in egl");
        return -1;
    }
}

void LoadTextureUtil::loadWidthHeightFromOption(IOptions *option) {
    if (option == nullptr || option->getAddress().empty()) {
        HLOGV("loadWidthHeightFromOption option null");
        return;
    }
    const char* fileName = option->getAddress().c_str();
    int* w = &option->srcWidth;
    int* h = &option->srcHeight;
    int* n = &option->srcChannel;
    //读取图片长宽高数据
    unsigned char* data = stbi_load(fileName, w, h, n, 0);
    if (data == nullptr) {
        HLOGE("%s is null", fileName);
    }

    HLOGV("loadTexture fileName = %s,width = %d,height=%d,n=%d",fileName,*w,*h,*n);
}

GLuint LoadTextureUtil::loadTextureFromOption(IOptions* option) {
    if (option == nullptr) {
        HLOGV("loadTextureFromFile option null");
        return -1;
    }

    GLuint textureHandler=0;
    glGenTextures(1,&textureHandler);
    if (textureHandler != GL_NONE){
        glBindTexture(GL_TEXTURE_2D,textureHandler);
        //纹理放大缩小使用线性插值
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //超出的部份会重复纹理坐标的边缘，产生一种边缘被拉伸的效果，s/t相当于x/y轴坐标
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        const char* fileName = option->getAddress().c_str();
        int* w = &option->srcWidth;
        int* h = &option->srcHeight;
        int* n = &option->srcChannel;

        //读取图片长宽高数据
        unsigned char* data = stbi_load(fileName, w, h, n, 0);

        HLOGV("loadTexture fileName = %s,width = %d,height=%d,n=%d",fileName,*w,*h,*n);

        if(data != nullptr) {
            if (*n == 3) { //判断是jpg格式
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *w, *h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else if (*n == 4) {  //判断是png格式
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *w, *h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            } else{
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *w, *h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            stbi_image_free(data);
            option->textureId = textureHandler;
            return textureHandler;
        } else {
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            HLOGE("load texture is null,fileName = %s",fileName);
            stbi_image_free(data);

            return 0; //代表加载图片失败
        }
    } else {
        HLOGE("glGenTextures 0 ,maybe not in egl");
        return -1;
    }
}