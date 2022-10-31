//
// Created by zengjiale on 2022/10/31.
//

#ifndef HRES_FBFILTER_H
#define HRES_FBFILTER_H
#include <filter/ifilter.h>

using namespace std;
class FbFilter: public IFilter {
public:

    FbFilter();

    ~FbFilter();

    void initFilter() override;

    void renderFrame() override;

    void clearFrame() override;

    void destroyFilter() override;

    void setOptions(IOptions *config) override;

    void updateViewPort(int width, int height) override;

    GLuint getExternalTexture() override;

    void releaseTexture() override;

    void swapBuffers() override;

    void enableFramebuffer();

    void disableFrameBuffer();

private:

    shared_ptr<GlFloatArray> vertexArray;
    shared_ptr<GlFloatArray> rgbaArray;

    GLuint shaderProgram;
    //顶点位置
    GLint uTextureLocation;
    //纹理位置
    GLint positionLocation;
    //纹理位置
    GLint textureLocation;


    int surfaceWidth = 0;
    int surfaceHeight = 0;

    GLuint fboId = -1;
    GLuint fboTextureId = -1;
};

#endif //HRES_FBFILTER_H
