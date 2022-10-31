//
// Created by asus on 2022/10/24.
//

#include "fbfilter.h"

FbFilter::FbFilter() {
    initFilter();
}

FbFilter::~FbFilter() {
    destroyFilter();
}

void FbFilter::initFilter() {
    char VERTEX_SHADER[] = "attribute vec4 vPosition;\n"
                           "attribute vec4 vTexCoordinate;\n"
                           "varying vec2 v_TexCoordinate;\n"
                           "\n"
                           "void main() {\n"
                           "    v_TexCoordinate = vec2(vTexCoordinate.x, vTexCoordinate.y);\n"
                           "    gl_Position = vPosition;\n"
                           "}";

    char FRAGMENT_SHADER[] = "precision mediump float;\n"
                             "uniform sampler2D uTexture;\n"
                             "varying vec2 v_TexCoordinate;\n"
                             "\n"
                             "void main () {\n"
                             "    gl_FragColor = texture2D(uTexture, v_TexCoordinate);\n"
                             //                             "    gl_FragColor = vec4(1.0,0.2,0.5,1.0);\n"
                             "}";
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");

    glGenTextures(1, &fboTextureId);
    glBindTexture(GL_TEXTURE_2D, fboTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    vertexArray = make_shared<GlFloatArray>();
    rgbaArray = make_shared<GlFloatArray>();
}

void FbFilter::renderFrame(int frameIndex) {
    if (fboTextureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboTextureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);
        rgbaArray->setVertexAttribPointer(textureLocation);

        glEnable(GL_BLEND);
        //基于alpha通道的半透明混合函数
        //void glBlendFuncSeparate(GLenum srcRGB,
        //     GLenum dstRGB,
        //     GLenum srcAlpha,
        //     GLenum dstAlpha);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //关闭混合
        glDisable(GL_BLEND);
    }
}

void FbFilter::clearFrame() {

}

void FbFilter::destroyFilter() {

}

void FbFilter::setoptions(IOptions *options) {
    if (options != nullptr) {
        glGenFramebuffers(1, &fboId);

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glBindTexture(GL_TEXTURE_2D, fboTextureId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, fboTextureId, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceWidth, surfaceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ELOGE("initFrameBuffer framebuffer init fail");
            return;
        }
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }

    vertexArray->setArray(VertexUtil::createFlip(options->getScaleWidth(), options->getScaleHeight(),
                                                 new PointRect(0, 0, options->getScaleWidth(),
                                                               options->getScaleHeight()), vertexArray->array));

    float *rgba = TexCoordsUtil::create(options->getScaleWidth(), options->getScaleHeight(),
                                        new PointRect(0, 0, options->getScaleWidth(),
                                                      options->getScaleHeight()),
                                        rgbaArray->array);
    rgbaArray->setArray(rgba);
}

void FbFilter::updateViewPort(int width, int height) {
    if (width <= 0 || height <= 0) return;
    surfaceWidth = width;
    surfaceHeight = height;
}

GLuint FbFilter::getExternalTexture() {
    return fboTextureId;
}

void FbFilter::releaseTexture() {
    glDeleteTextures(1, &fboTextureId);
    glDeleteFramebuffers(1, &fboId);
}

void FbFilter::swapBuffers() {

}

void FbFilter::enableFramebuffer() {
    if (fboId != -1) {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    }
}

void FbFilter::disableFrameBuffer() {
    if (fboId != -1) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
