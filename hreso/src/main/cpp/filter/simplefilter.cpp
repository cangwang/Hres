//
// Created by cangwang on 2022/10/19.
//

#include "simplefilter.h"

SimpleFilter::SimpleFilter(string type): vertexArray(new GlFloatArray()), rgbaArray(new GlFloatArray()) {
    setShader(type);
}

SimpleFilter::~SimpleFilter() {
    vertexArray = nullptr;
    rgbaArray = nullptr;
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
}

void SimpleFilter::setShader(string type) {
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();

    VERTEX_SHADER =
            "#version 300 es\n"
            "in vec4 vPosition;\n"
            "in vec4 vTexCoordinate;\n"
            "out vec2 v_TexCoordinate;\n"
            "\n"
            "void main() {\n"
            "v_TexCoordinate = vec2(vTexCoordinate.x, vTexCoordinate.y);\n"
            "gl_Position = vPosition;\n"
            "}";

    if (type == "simple") {
        FRAGMENT_SHADER = "#version 300 es\n"
                          //                      "#extension GL_OES_EGL_image_external_essl3 : require"
                          "precision mediump float;\n"
                          "uniform sampler2D uTexture;\n"
                          "in vec2 v_TexCoordinate;\n"
                          "out vec4 vFragColor;\n"
                          "\n"
                          "void main () {\n"
                          "vFragColor = texture(uTexture, v_TexCoordinate);\n"
                          //                      "vFragColor = vec4(1.0,0.2,0.5,1.0);\n"
                          "}";
    }
    initFilter();
}

void SimpleFilter::initFilter() {
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");
}

void SimpleFilter::renderFrame() {
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

void SimpleFilter::clearFrame() {

}

void SimpleFilter::destroyFilter() {
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
    releaseTexture();
}

void SimpleFilter::setOptions(IOptions *options) {
    if (options) {
        textureId = options->textureId;
        vertexArray->setArray(
                VertexUtil::create(options->getScaleWidth(), options->getScaleHeight(),
                                   new PointRect(0, 0, options->getScaleWidth(),
                                                 options->getScaleHeight()), vertexArray->array));
        float *rgba = TexCoordsUtil::create(options->getScaleWidth(), options->getScaleHeight(),
                                            new PointRect(0, 0, options->getScaleWidth(),
                                                          options->getScaleHeight()),
                                            rgbaArray->array);
        rgbaArray->setArray(rgba);
    } else {
        HLOGE("options is null");
    }
}

void SimpleFilter::updateViewPort(int width, int height) {
    surfaceWidth = width;
    surfaceHeight = height;
}

GLuint SimpleFilter::getExternalTexture() {
    return textureId;
}

void SimpleFilter::releaseTexture() {
    if (textureId != -1) {
        glDeleteTextures(1,&textureId);
    }
}

void SimpleFilter::swapBuffers() {

}
