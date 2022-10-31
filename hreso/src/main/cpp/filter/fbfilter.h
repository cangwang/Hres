//
// Created by zengjiale on 2022/10/31.
//

#ifndef HRES_FBFILTER_H
#define HRES_FBFILTER_H
#include <filter/ifilter.h>
#include <util/stb_image_write.h>
#include <util/threadpool.h>
#include <filter/filterlistener.h>

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

    void setListener(FilterListener* listener);

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
    GLuint pixelBuffer;

    IOptions* option;
    unsigned char* saveImgData;
    long imgSize;
    FilterListener* listener;
    ThreadPool* pool;

    void initPixelBuffer();
    void drawPixelBuffer();
    void destroyPixelBuffers();
    void save(IOptions* option);
    void saveInThread(IOptions* option);
    bool saveImg(const string saveFileAddress,unsigned char* data,int width,int height,int type);
};

#endif //HRES_FBFILTER_H
