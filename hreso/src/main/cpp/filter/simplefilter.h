//
// Created by asus on 2022/10/19.
//

#ifndef HRES_SIMPLEFILTER_H
#define HRES_SIMPLEFILTER_H

#include <filter/ifilter.h>

class SimpleFilter : public IFilter {
public:
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
    GlFloatArray *vertexArray;
    GlFloatArray *rgbaArray;

    GLuint shaderProgram;
    //shader
    GLuint textureId = -1;
    //顶点位置
    GLint uTextureLocation;
    //纹理位置
    GLint positionLocation;
    //纹理位置
    GLint textureLocation;

    int surfaceWidth = 0;
    int surfaceHeight = 0;

};


#endif //HRES_SIMPLEFILTER_H
