//
// Created by zengjiale on 2022/11/2.
//

#include "FsrUpFilter.h"

FsrUpFilter::FsrUpFilter(string type): vertexArray(new GlFloatArray()), rgbaArray(new GlFloatArray()) {
    setShader(type);
}

FsrUpFilter::~FsrUpFilter() {
    vertexArray = nullptr;
    rgbaArray = nullptr;
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
}

void FsrUpFilter::setShader(string type) {
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();

    VERTEX_SHADER =
            "#version 300 es\n"
            "precision highp float;\n"
            "in vec4 vPosition;\n"
            "in vec4 vTexCoordinate;\n"
            "out vec2 v_TexCoordinate;\n"
            "\n"
            "void main() {\n"
            "   v_TexCoordinate = vec2(vTexCoordinate.x, vTexCoordinate.y);\n"
            "   gl_Position = vPosition;\n"
            "}";

    FRAGMENT_SHADER =
            "#version 300 es\n"
            "precision highp float;\n"
            "in highp vec2 v_TexCoordinate;\n"
            "uniform float w;\n"
            "uniform float h;\n"
            "uniform float iW;\n"
            "uniform float iH;\n"

            "uniform sampler2D uTexture;\n"
            "out vec4 glFragColor;\n"

            "void main() {\n"
            "    vec4 scale = vec4(1. / vec2(iW, iH), vec2(iW, iH) / vec2(w,h));\n"

            // Source position in fractions of a texel
            "    vec2 src_pos = scale.zw * (v_TexCoordinate * vec2(w,h));\n"
            // Source bottom left texel centre
            "    vec2 src_centre = floor(src_pos - .5) + .5;\n"
            // f is position. f.x runs left to right, y bottom to top, z right to left, w top to bottom
            "    vec4 f; f.zw = 1. - (f.xy = src_pos - src_centre);\n"
            // Calculate weights in x and y in parallel.
            // These polynomials are piecewise approximation of Lanczos kernel
            // Calculator here: https://gist.github.com/going-digital/752271db735a07da7617079482394543
            "    vec4 l2_w0_o3 = ((1.5672 * f - 2.6445) * f + 0.0837) * f + 0.9976;\n"
            "    vec4 l2_w1_o3 = ((-0.7389 * f + 1.3652) * f - 0.6295) * f - 0.0004;\n"

            "    vec4 w1_2 = l2_w0_o3;\n"
            "    vec2 w12 = w1_2.xy + w1_2.zw;\n"
            "    vec4 wedge = l2_w1_o3.xyzw * w12.yxyx;\n"
            // Calculate texture read positions. tc12 uses bilinear interpolation to do 4 reads in 1.
            "    vec2 tc12 = scale.xy * (src_centre + w1_2.zw / w12);\n"
            "    vec2 tc0 = scale.xy * (src_centre - 1.);\n"
            "    vec2 tc3 = scale.xy * (src_centre + 2.);\n"
            // Sharpening adjustment
            // Thanks mad_gooze for the normalization fix.
            "    float sum = wedge.x + wedge.y + wedge.z + wedge.w + w12.x * w12.y;\n"
            "    wedge /= sum;\n"
            "    vec3 col = vec3(texture(uTexture, vec2(tc12.x, tc0.y)).rgb * wedge.y + "
            "       texture(uTexture, vec2(tc0.x, tc12.y)).rgb * wedge.x + "
            "       texture(uTexture, tc12.xy).rgb * (w12.x * w12.y/sum) + "
            "       texture(uTexture, vec2(tc3.x, tc12.y)).rgb * wedge.z + "
            "       texture(uTexture, vec2(tc12.x, tc3.y)).rgb * wedge.w);\n"

            "    glFragColor = vec4(col,1);\n"
            "}";
    initFilter();
}


void FsrUpFilter::initFilter() {
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");
    wLocation = glGetUniformLocation(shaderProgram,"w");
    hLocation = glGetUniformLocation(shaderProgram,"h");
    iWLocation = glGetUniformLocation(shaderProgram,"iW");
    iHLocation = glGetUniformLocation(shaderProgram,"iH");
}

void FsrUpFilter::renderFrame() {
    if (textureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);
        rgbaArray->setVertexAttribPointer(textureLocation);

        glUniform1f(iWLocation, surfaceWidth);
        glUniform1f(iHLocation, surfaceHeight);
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

void FsrUpFilter::clearFrame() {

}

void FsrUpFilter::destroyFilter() {
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
    releaseTexture();
}

void FsrUpFilter::setOptions(IOptions *options) {
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
        imageWidth = (float) options->srcWidth;
        imageHeight = (float) options->srcHeight;
        surfaceWidth = (float)options->getScaleWidth();
        surfaceHeight = (float)options->getScaleHeight();
        HLOGV("fsrupFilter %f, %f, %f, %f", imageWidth, imageHeight, surfaceWidth, surfaceHeight);
    } else {
        HLOGE("options is null");
    }
}

void FsrUpFilter::updateViewPort(int width, int height) {
}

GLuint FsrUpFilter::getExternalTexture() {
    return textureId;
}

void FsrUpFilter::releaseTexture() {
    if (textureId != -1) {
        glDeleteTextures(1,&textureId);
    }
}

void FsrUpFilter::swapBuffers() {

}
