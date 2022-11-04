//
// Created by zengjiale on 2022/11/2.
//

#include "fsrfilter.h"

FsrFilter::FsrFilter(string type): vertexArray(new GlFloatArray()), rgbaArray(new GlFloatArray()) {
    setShader(type);
}

FsrFilter::~FsrFilter() {
    vertexArray = nullptr;
    rgbaArray = nullptr;
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
}

void FsrFilter::setShader(string type) {
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
    } else if (type == "hermite") {
        FRAGMENT_SHADER =
                "#version 300 es\n"
                "precision highp float;\n"
                "in highp vec2 v_TexCoordinate;\n"
                "uniform sampler2D uTexture;\n"
                "out vec4 glFragColor;\n"

                "uniform float w;\n"
                "uniform float h;\n"
                "float c_textureSize = 0.0;\n"

                "float c_onePixel = 0.0;\n"
                "float c_twoPixels = 0.0;\n"

                "float c_x0 = -1.0;\n"
                "float c_x1 =  0.0;\n"
                "float c_x2 =  1.0;\n"
                "float c_x3 =  2.0;\n"

                //=======================================================================================
                "vec3 CubicHermite (vec3 A, vec3 B, vec3 C, vec3 D, float t)\n"
                "{\n"
                "float t2 = t*t;\n"
                "float t3 = t*t*t;\n"
                "vec3 a = -A/2.0 + (3.0*B)/2.0 - (3.0*C)/2.0 + D/2.0;\n"
                "vec3 b = A - (5.0*B)/2.0 + 2.0*C - D / 2.0;\n"
                "vec3 c = -A/2.0 + C/2.0;\n"
                "vec3 d = B;\n"

                "return a*t3 + b*t2 + c*t + d;\n"
                "}\n"

                //=======================================================================================
                "vec3 BicubicHermiteTextureSample (vec2 P)\n"
                "{\n"
                "vec2 pixel = P * c_textureSize + 0.5;\n"

                "vec2 frac = fract(pixel);\n"
                "pixel = floor(pixel) / c_textureSize - vec2(c_onePixel/2.0);\n"

                "vec3 C00 = texture(uTexture, pixel + vec2(-c_onePixel ,-c_onePixel)).rgb;\n"
                "vec3 C10 = texture(uTexture, pixel + vec2( 0.0        ,-c_onePixel)).rgb;\n"
                "vec3 C20 = texture(uTexture, pixel + vec2( c_onePixel ,-c_onePixel)).rgb;\n"
                "vec3 C30 = texture(uTexture, pixel + vec2( c_twoPixels,-c_onePixel)).rgb;\n"

                "vec3 C01 = texture(uTexture, pixel + vec2(-c_onePixel , 0.0)).rgb;\n"
                "vec3 C11 = texture(uTexture, pixel + vec2( 0.0        , 0.0)).rgb;\n"
                "vec3 C21 = texture(uTexture, pixel + vec2( c_onePixel , 0.0)).rgb;\n"
                "vec3 C31 = texture(uTexture, pixel + vec2( c_twoPixels, 0.0)).rgb;\n"

                "vec3 C02 = texture(uTexture, pixel + vec2(-c_onePixel , c_onePixel)).rgb;\n"
                "vec3 C12 = texture(uTexture, pixel + vec2( 0.0        , c_onePixel)).rgb;\n"
                "vec3 C22 = texture(uTexture, pixel + vec2( c_onePixel , c_onePixel)).rgb;\n"
                "vec3 C32 = texture(uTexture, pixel + vec2( c_twoPixels, c_onePixel)).rgb;\n"

                "vec3 C03 = texture(uTexture, pixel + vec2(-c_onePixel , c_twoPixels)).rgb;\n"
                "vec3 C13 = texture(uTexture, pixel + vec2( 0.0        , c_twoPixels)).rgb;\n"
                "vec3 C23 = texture(uTexture, pixel + vec2( c_onePixel , c_twoPixels)).rgb;\n"
                "vec3 C33 = texture(uTexture, pixel + vec2( c_twoPixels, c_twoPixels)).rgb;\n"

                "vec3 CP0X = CubicHermite(C00, C10, C20, C30, frac.x);\n"
                "vec3 CP1X = CubicHermite(C01, C11, C21, C31, frac.x);\n"
                "vec3 CP2X = CubicHermite(C02, C12, C22, C32, frac.x);\n"
                "vec3 CP3X = CubicHermite(C03, C13, C23, C33, frac.x);\n"

                "return CubicHermite(CP0X, CP1X, CP2X, CP3X, frac.y);\n"
                "}\n"

                "void main() {\n"
                "c_textureSize = sqrt(w*h);\n"
                "c_onePixel = (1.0 / c_textureSize);\n"
                "c_twoPixels = (2.0 / c_textureSize);\n"
                "glFragColor = vec4(BicubicHermiteTextureSample(v_TexCoordinate), 1.0);\n"
                "}";
    } else if (type == "lagrange") {
        FRAGMENT_SHADER =
                "#version 300 es\n"
                "precision highp float;\n"
                "in highp vec2 v_TexCoordinate;\n"
                "uniform sampler2D uTexture;\n"
                "out vec4 glFragColor;\n"
                "uniform float w;\n"
                "uniform float h;\n"
                "float c_textureSize = 0.0;\n"

                "float c_onePixel = 0.0;\n"
                "float c_twoPixels = 0.0;\n"
                "float c_x0 = -1.0;\n"
                "float c_x1 =  0.0;\n"
                "float c_x2 =  1.0;\n"
                "float c_x3 =  2.0;\n"

                "vec3 CubicLagrange (vec3 A, vec3 B, vec3 C, vec3 D, float t) {\n"
                "return\n"
                "    A * (\n"
                "            (t - c_x1) / (c_x0 - c_x1) *\n"
                "            (t - c_x2) / (c_x0 - c_x2) *\n"
                "            (t - c_x3) / (c_x0 - c_x3)) +\n"
                "    B * (\n"
                "            (t - c_x0) / (c_x1 - c_x0) *\n"
                "            (t - c_x2) / (c_x1 - c_x2) *\n"
                "            (t - c_x3) / (c_x1 - c_x3)) +\n"
                "    C * (\n"
                "            (t - c_x0) / (c_x2 - c_x0) *\n"
                "            (t - c_x1) / (c_x2 - c_x1) *\n"
                "            (t - c_x3) / (c_x2 - c_x3)) +\n"
                "    D * (\n"
                "            (t - c_x0) / (c_x3 - c_x0) *\n"
                "            (t - c_x1) / (c_x3 - c_x1) *\n"
                "            (t - c_x2) / (c_x3 - c_x2)\n"
                "    );\n"
                "}\n"

                //=======================================================================================
                "vec3 BicubicLagrangeTextureSample (vec2 P) {\n"
                "vec2 pixel = P * c_textureSize + 0.5;\n"

                "vec2 frac = fract(pixel);\n"
                "pixel = floor(pixel) / c_textureSize - vec2(c_onePixel/2.0);\n"

                "vec3 C00 = texture(uTexture, pixel + vec2(-c_onePixel ,-c_onePixel)).rgb;\n"
                "vec3 C10 = texture(uTexture, pixel + vec2( 0.0        ,-c_onePixel)).rgb;\n"
                "vec3 C20 = texture(uTexture, pixel + vec2( c_onePixel ,-c_onePixel)).rgb;\n"
                "vec3 C30 = texture(uTexture, pixel + vec2( c_twoPixels,-c_onePixel)).rgb;\n"

                "vec3 C01 = texture(uTexture, pixel + vec2(-c_onePixel , 0.0)).rgb;\n"
                "vec3 C11 = texture(uTexture, pixel + vec2( 0.0        , 0.0)).rgb;\n"
                "vec3 C21 = texture(uTexture, pixel + vec2( c_onePixel , 0.0)).rgb;\n"
                "vec3 C31 = texture(uTexture, pixel + vec2( c_twoPixels, 0.0)).rgb;\n"

                "vec3 C02 = texture(uTexture, pixel + vec2(-c_onePixel , c_onePixel)).rgb;\n"
                "vec3 C12 = texture(uTexture, pixel + vec2( 0.0        , c_onePixel)).rgb;\n"
                "vec3 C22 = texture(uTexture, pixel + vec2( c_onePixel , c_onePixel)).rgb;\n"
                "vec3 C32 = texture(uTexture, pixel + vec2( c_twoPixels, c_onePixel)).rgb;\n"

                "vec3 C03 = texture(uTexture, pixel + vec2(-c_onePixel , c_twoPixels)).rgb;\n"
                "vec3 C13 = texture(uTexture, pixel + vec2( 0.0        , c_twoPixels)).rgb;\n"
                "vec3 C23 = texture(uTexture, pixel + vec2( c_onePixel , c_twoPixels)).rgb;\n"
                "vec3 C33 = texture(uTexture, pixel + vec2( c_twoPixels, c_twoPixels)).rgb;\n"

                "vec3 CP0X = CubicLagrange(C00, C10, C20, C30, frac.x);\n"
                "vec3 CP1X = CubicLagrange(C01, C11, C21, C31, frac.x);\n"
                "vec3 CP2X = CubicLagrange(C02, C12, C22, C32, frac.x);\n"
                "vec3 CP3X = CubicLagrange(C03, C13, C23, C33, frac.x);\n"

                "return CubicLagrange(CP0X, CP1X, CP2X, CP3X, frac.y);\n"
                "}\n"


                "void main() {\n"
                "c_textureSize = sqrt(w*h);\n"
                "c_onePixel = (1.0 / c_textureSize);\n"
                "c_twoPixels = (2.0 / c_textureSize);\n"
                "glFragColor = vec4(BicubicLagrangeTextureSample(v_TexCoordinate), 1.0);\n"
                "}";
    } else if (type == "fsr") {
        FRAGMENT_SHADER =
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
    }
    initFilter();
}


void FsrFilter::initFilter() {
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
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
    releaseTexture();
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
