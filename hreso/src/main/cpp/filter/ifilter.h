//
// Created by cangwang on 2022/10/16.
//

#ifndef HRES_IFILTER_H
#define HRES_IFILTER_H

#include <GLES3/gl3.h>
#include <transform/ioptions.h>
#include <util/shaderutil.h>
#include <util/glfloatarray.h>
#include <util/vertexutil.h>
#include <util/texcoordsutil.h>
#include <util/loadtextureutil.h>

#define SHADER_STR(s) #s

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
