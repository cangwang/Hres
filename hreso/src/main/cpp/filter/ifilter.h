//
// Created by asus on 2022/10/16.
//

#ifndef HRES_IFILTER_H
#define HRES_IFILTER_H
#include <GLES2/gl2.h>
#include <transform/ioptions.h>
#include <util/shaderutil.h>

class IFilter {
public:
    virtual void initFilter() = 0;
    virtual void renderFrame() = 0;
    virtual void clearFrame() = 0;
    virtual void destroyFilter() = 0;
    virtual void setOptions(IOptions* config) = 0;
    virtual void updateViewPort(int width, int height) {};
    virtual GLuint getExternalTexture() = 0;
    virtual void releaseTexture() = 0;
    virtual void swapBuffers() = 0;
};

#endif //HRES_IFILTER_H
