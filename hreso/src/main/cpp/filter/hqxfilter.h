//
// Created by cangwang on 2022/10/19.
//

#ifndef HRES_HqxFilter_H
#define HRES_HqxFilter_H

#include <filter/ifilter.h>

#define LOG_TAG "HqxFilter"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

class HqxFilter : public IFilter {
public:
    HqxFilter(string type);
    ~HqxFilter();

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
    //放大倍数
    GLint upscaleLocation;

    int surfaceWidth = 0;
    int surfaceHeight = 0;
    float scale = 1.0;

    string VERTEX_SHADER;
    string FRAGMENT_SHADER;

    void setShader(string type);
};


#endif //HRES_HqxFilter_H
