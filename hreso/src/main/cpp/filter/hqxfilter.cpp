//
// Created by zengjiale on 2022/11/4.
//

#include "hqxfilter.h"

HqxFilter::HqxFilter(string type): vertexArray(new GlFloatArray()), rgbaArray(new GlFloatArray()) {
    setShader(type);
}

HqxFilter::~HqxFilter() {
    vertexArray = nullptr;
    rgbaArray = nullptr;
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
}

void HqxFilter::setShader(string type) {
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

    if (type == "hqx") {
        FRAGMENT_SHADER =
                "#version 300 es\n"
                "precision highp float;\n"
                "in highp vec2 v_TexCoordinate;\n"
                "uniform sampler2D uTexture;\n"
                "out vec4 glFragColor;\n"

                //upscaling multiplier amount
                "uniform float upscale;\n"

                //image mipmap level, for base upscaling
                "#define ML 0\n"

                //equality threshold of 2 colors before forming lines
                "#define THRESHOLD .1\n"

                //line thickness
                "float LINE_THICKNESS;\n"

                //anti aliasing scaling, smaller value make lines more blurry
                //#define AA_SCALE (upscale*1.)

                "bool diag(inout vec4 sum, vec2 uv, vec2 p1, vec2 p2) {\n"
                "vec4 v1 = texelFetch(uTexture,ivec2(uv+vec2(p1.x,p1.y)),ML),\n"
                "    v2 = texelFetch(uTexture,ivec2(uv+vec2(p2.x,p2.y)),ML);\n"
                "if (length(v1-v2) < THRESHOLD) {\n"
                "vec2 dir = p2-p1,\n"
                "lp = uv-(floor(uv+p1)+.5);\n"
                "dir = normalize(vec2(dir.y,-dir.x));\n"
                "float l = clamp((LINE_THICKNESS-dot(lp,dir))*upscale, 0., 1.);\n"
                "sum = mix(sum,v1,l);\n"
                "return true;\n"
                "}\n"
                "return false;\n"
                "}\n"

                "void main() {\n"
                "vec2 ip = v_TexCoordinate/upscale;\n"

                //start with nearest pixel as 'background'
                "vec4 s = texelFetch(uTexture,ivec2(ip),ML);\n"

                //draw anti aliased diagonal lines of surrounding pixels as 'foreground'
                "LINE_THICKNESS = .4;\n"
                "if (diag(s,ip,vec2(-1, 0),vec2(0, 1))) {\n"
                "LINE_THICKNESS = .3;\n"
                "diag(s,ip,vec2(-1, 0),vec2(1, 1));\n"
                "diag(s,ip,vec2(-1, -1),vec2(0, 1));\n"
                "}\n"
                "LINE_THICKNESS = .4;\n"
                "if (diag(s,ip,vec2(0, 1),vec2(1, 0))) {\n"
                "LINE_THICKNESS = .3;\n"
                "diag(s,ip,vec2(0,1),vec2(1, -1));\n"
                "diag(s,ip,vec2(-1, 1),vec2(1, 0));\n"
                "}\n"
                "LINE_THICKNESS = .4;\n"
                "if (diag(s,ip,vec2(1, 0),vec2(0, -1))) {\n"
                "LINE_THICKNESS = .3;\n"
                "diag(s,ip,vec2(1, 0),vec2(-1, -1));\n"
                "diag(s,ip,vec2(1, 1),vec2(0, -1));\n"
                "}\n"
                "LINE_THICKNESS = .4;\n"
                "if (diag(s,ip,vec2(0, -1),vec2(-1, 0))) {\n"
                "LINE_THICKNESS = .3;\n"
                "diag(s,ip,vec2(0, -1),vec2(-1, 1));\n"
                "diag(s,ip,vec2(1, -1),vec2(-1, 0));\n"
                " }\n"

                "glFragColor = s;\n"
                "}";
    }
    initFilter();
}

void HqxFilter::initFilter() {
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");
    upscaleLocation = glGetUniformLocation(shaderProgram, "upscale");
}

void HqxFilter::renderFrame() {
    if (textureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);
        rgbaArray->setVertexAttribPointer(textureLocation);

        glUniform1f(upscaleLocation, scale);
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

void HqxFilter::clearFrame() {

}

void HqxFilter::destroyFilter() {
    VERTEX_SHADER.clear();
    FRAGMENT_SHADER.clear();
    releaseTexture();
}

void HqxFilter::setOptions(IOptions *options) {
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
        scale = options->getScaleRatio();
    } else {
        HLOGE("options is null");
    }
}

void HqxFilter::updateViewPort(int width, int height) {
    surfaceWidth = width;
    surfaceHeight = height;
}

GLuint HqxFilter::getExternalTexture() {
    return textureId;
}

void HqxFilter::releaseTexture() {
    if (textureId != -1) {
        glDeleteTextures(1,&textureId);
    }
}

void HqxFilter::swapBuffers() {

}
