#include <GLES3/gl3.h>

#ifndef HRES_GLBASE_H
#define HRES_GLBASE_H

/**
 * cangwang 2018.12.1
 */
class GLBase {
public:
    GLBase(): mProgram(0), mWidth(0), mHeight(0) {

    }

    void resize(int width,int height){
        mWidth = width;
        mHeight = height;
    }

protected:
    GLuint mProgram;
    GLint mWidth;
    GLint mHeight;

private:

};

#endif
