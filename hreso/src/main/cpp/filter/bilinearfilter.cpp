//
// Created by cangwang on 2022/10/16.
//

#include "bilinearfilter.h"

BilinearFilter::BilinearFilter() {
    initFilter();
}

BilinearFilter::~BilinearFilter() {

}

void BilinearFilter::initFilter() {
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
}

void BilinearFilter::renderFrame() {
//    if (surfaceSizeChanged && surfaceWidth > 0 && surfaceHeight > 0) {
//        surfaceSizeChanged = false;
//        glViewport(0, 0, surfaceWidth, surfaceHeight);
//    }
    if (textureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
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

void BilinearFilter::clearFrame() {
    glClearColor(0, 0, 0, 0);

    glClear(GL_COLOR_BUFFER_BIT);
}

void BilinearFilter::destroyFilter() {
    releaseTexture();
}

void BilinearFilter::setOptions(IOptions *option) {

}

GLuint BilinearFilter::getExternalTexture() {
    return textureId;
}

void BilinearFilter::releaseTexture() {
    if (textureId > -1) {
        glDeleteTextures(1, &textureId);
    }
}

void BilinearFilter::swapBuffers() {

}
