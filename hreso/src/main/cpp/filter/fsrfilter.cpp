//
// Created by zengjiale on 2022/11/2.
//

#include "fsrfilter.h"

FsrFilter::FsrFilter(): vertexArray(new GlFloatArray()), rgbaArray(new GlFloatArray()) {
    initFilter();
}

FsrFilter::~FsrFilter() {
    vertexArray = nullptr;
    rgbaArray = nullptr;
}

void FsrFilter::initFilter() {
    char VERTEX_SHADER[] =
            "#version 300 es\n"
            "in vec4 vPosition;\n"
            "in vec4 vTexCoordinate;\n"
            "out vec2 v_TexCoordinate;\n"
            "\n"
            "void main() {\n"
            "v_TexCoordinate = vec2(vTexCoordinate.x, vTexCoordinate.y);\n"
            "gl_Position = vPosition;\n"
            "}";

    char FRAGMENT_SHADER[] =
    "#version 300 es\n"
    "precision highp float;\n"
    "in highp vec2 v_TexCoordinate;\n"
    "uniform float w;\n"
    "uniform float h;\n"

    "uniform sampler2D uTexture;\n"
    "out vec4 glFragColor;\n"

    "void main() {\n"
        /// Normalized pixel coordinates (from 0 to 1)
    "vec2 uv = v_TexCoordinate;\n"

        // Time varying pixel color
    "vec3 col = texture(uTexture, uv).xyz;\n"

        // CAS algorithm
    "    float max_g = col.y;\n"
    "    float min_g = col.y;\n"
    "    vec4 uvoff = vec4(1,0,1,-1)/vec4(w, w, h, h);\n"
    "    vec3 colw;\n"
    "    vec3 col1 = texture(uTexture, uv+uvoff.yw).xyz;\n"
    "    max_g = max(max_g, col1.y);\n"
    "    min_g = min(min_g, col1.y);\n"
    "    colw = col1;\n"
    "    col1 = texture(uTexture, uv+uvoff.xy).xyz;\n"
    "    max_g = max(max_g, col1.y);\n"
    "    min_g = min(min_g, col1.y);\n"
    "    colw += col1;\n"
    "    col1 = texture(uTexture, uv+uvoff.yz).xyz;\n"
    "    max_g = max(max_g, col1.y);\n"
    "    min_g = min(min_g, col1.y);\n"
    "    colw += col1;\n"
    "    col1 = texture(uTexture, uv-uvoff.xy).xyz;\n"
    "    max_g = max(max_g, col1.y);\n"
    "    min_g = min(min_g, col1.y);\n"
    "    colw += col1;\n"
    "    float d_min_g = min_g;\n"
    "    float d_max_g = 1.-max_g;\n"
//    "    float A;\n"
//    "    if (d_max_g < d_min_g) {\n"
//    "        A = d_max_g / max_g;\n"
//    "    } else {\n"
//    "        A = d_min_g / max_g;\n"
//    "    }\n"
    "    float A = step(d_min_g, d_max_g) / max_g;"
    "    A = sqrt(A);\n"
    "    A *= mix(-.125, -.2, 1.0);\n"
    "    vec3 col_out = (col + colw * A) / (1.+4.*A);\n"
    "    glFragColor = vec4(col_out,1);\n"
    "}";
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");
    wLocation = glGetUniformLocation(shaderProgram,"w");
    hLocation = glGetUniformLocation(shaderProgram,"h");
}

void FsrFilter::renderFrame() {
    if (textureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);
        rgbaArray->setVertexAttribPointer(textureLocation);

        glUniform1f(wLocation, surfaceWidth);
        glUniform1f(hLocation, surfaceHeight);
        checkGLError("glUniform1i");

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

void FsrFilter::clearFrame() {

}

void FsrFilter::destroyFilter() {
//    VERTEX_SHADER.clear();
//    FRAGMENT_SHADER.clear();
}

void FsrFilter::setOptions(IOptions *options) {
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
        surfaceWidth = (float)options->getScaleWidth();
        surfaceHeight = (float)options->getScaleHeight();
    } else {
        HLOGE("options is null");
    }
}

void FsrFilter::updateViewPort(int width, int height) {
}

GLuint FsrFilter::getExternalTexture() {
    return textureId;
}

void FsrFilter::releaseTexture() {
    if (textureId != -1) {
        glDeleteTextures(1,&textureId);
    }
}

void FsrFilter::swapBuffers() {

}
