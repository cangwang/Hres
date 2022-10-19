//
// Created by asus on 2022/10/19.
//

#include "simplefilter.h"

void SimpleFilter::initFilter() {
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

void SimpleFilter::renderFrame() {

}

void SimpleFilter::clearFrame() {

}

void SimpleFilter::destroyFilter() {

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
