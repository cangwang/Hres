//
// Created by asus on 2022/10/19.
//

#include "simplefilter.h"

void SimpleFilter::initFilter() {
    VERTEX_SHADER = SHADER_STR(
            attribute vec4 vPosition;
            attribute vec4 vTexCoordinate;
            varying vec2 v_TexCoordinate;
            void main() {
                v_TexCoordinate = vec2(vTexCoordinate.x, vTexCoordinate.y);
                gl_Position = vPosition;
            }
    );

    FRAGMENT_SHADER = SHADER_STR(
            precision mediump float;
            uniform sampler2D uTexture;
            varying vec2 v_TexCoordinate;
            void main () {
                gl_FragColor = texture2D(uTexture, v_TexCoordinate);
//                gl_FragColor = vec4(1.0,0.2,0.5,1.0);
            }
    );
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
}

void SimpleFilter::setOptions(IOptions *config) {

}

void SimpleFilter::updateViewPort(int width, int height) {
    IFilter::updateViewPort(width, height);
}

GLuint SimpleFilter::getExternalTexture() {
    return 0;
}

void SimpleFilter::releaseTexture() {

}

void SimpleFilter::swapBuffers() {

}
