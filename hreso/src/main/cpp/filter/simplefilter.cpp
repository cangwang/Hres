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
