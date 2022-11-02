//
// Created by zengjiale on 2022/11/2.
//

#ifndef HRES_FSRFILTER_H
#define HRES_FSRFILTER_H
#include <filter/ifilter.h>

#define LOG_TAG "FsrFilter"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

class FsrFilter: public IFilter {
public:
    FsrFilter();
    ~FsrFilter();

    void initFilter() override;

    void renderFrame() override;

    void clearFrame() override;

    void destroyFilter() override;

    void setOptions(IOptions *config) override;

    void updateViewPort(int width, int height) override;

    GLuint getExternalTexture() override;

    void releaseTexture() override;

    void swapBuffers() override;
private:
    unique_ptr<GlFloatArray> vertexArray;
    unique_ptr<GlFloatArray> rgbaArray;

    GLuint shaderProgram;
    //shader
    GLuint textureId = -1;
    //顶点位置
    GLint uTextureLocation;
    //纹理位置
    GLint positionLocation;
    //纹理位置
    GLint textureLocation;
    //
    GLint wLocation;
    GLint hLocation;

    float surfaceWidth;
    float surfaceHeight;
};


#endif //HRES_FSRFILTER_H
