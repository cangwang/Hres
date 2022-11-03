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

void FbFilter::renderFrame() {
    if (fboTextureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboTextureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);
        rgbaArray->setVertexAttribPointer(textureLocation);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        checkGLError("glDrawArrays");
        drawPixelBuffer();
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

//        glEnable(GL_BLEND);
//        //基于alpha通道的半透明混合函数
//        //void glBlendFuncSeparate(GLenum srcRGB,
//        //     GLenum dstRGB,
//        //     GLenum srcAlpha,
//        //     GLenum dstAlpha);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//        drawPixelBuffer();
//        //关闭混合
//        glDisable(GL_BLEND);
    }
}

void FbFilter::clearFrame() {

}

void FbFilter::destroyFilter() {
    releaseTexture();
    destroyPixelBuffers();
}

void FbFilter::setOptions(IOptions *options) {
    if (options != nullptr) {
        this->option = options;
        glGenFramebuffers(1, &fboId);

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glBindTexture(GL_TEXTURE_2D, fboTextureId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, fboTextureId, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, options->getScaleWidth(), options->getScaleHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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
    initPixelBuffer();
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
    if (fboTextureId > 0) {
        glDeleteTextures(1, &fboTextureId);
    }
    if (fboId > 0) {
        glDeleteFramebuffers(1, &fboId);
    }
    option = nullptr;
    if (saveImgData) {
//        memset(saveImgData, 0, imgSize);
        saveImgData = nullptr;
    }
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

void FbFilter::initPixelBuffer() {
    destroyPixelBuffers();
    HLOGV("initPixelBuffer");
    pixelBuffer = -1;

    int align = 128;//128字节对齐
    imgSize = ((option->getScaleWidth() * 4 + (align - 1)) & ~(align - 1)) * option->getScaleHeight();
//    imgSize = option->getScaleWidth() * option->getScaleHeight() * 4;
    saveImgData = (unsigned char *) malloc(sizeof(unsigned char)*imgSize);
    //清空数据
    memset(saveImgData,0, sizeof(unsigned char)*imgSize);

    glGenBuffers(1, &pixelBuffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
    glBufferData(GL_PIXEL_PACK_BUFFER, imgSize, nullptr, GL_STATIC_READ);
}

void FbFilter::drawPixelBuffer() {
    HLOGV("drawPixelBuffer");
    if (pixelBuffer < 0) {
        HLOGE("pixelBuffer not init");
        return;
    }
    if (option != nullptr && option->getScaleWidth() > 0 && option->getScaleHeight() > 0) {
        glReadBuffer(GL_BACK);
        checkGLError("glReadBuffer");
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
        checkGLError("glBindBuffer");
        glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
        checkGLError("glPixelStorei");
        glReadPixels(0, 0, option->getScaleWidth(), option->getScaleHeight(), GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        checkGLError("glReadPixels");

        if (imgSize > 0) {
            HLOGV("imgSize = %ld",  imgSize);
            saveImgData = (unsigned char *) glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, imgSize,
                                                             GL_MAP_READ_BIT);
            checkGLError("glMapBufferRange");
        } else {
            HLOGE("imgSize = 0");
        }
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        //解除绑定PBO
        glBindBuffer(GL_PIXEL_PACK_BUFFER, GL_NONE);
    } else {
        HLOGE("scaleImageWith or scaleImageHeight = 0");
        return;
    }
}

string FbFilter::save() {
    string address;
    string error;
    if (option != nullptr) {
        if (option->getSaveAddress().empty()) {
            HLOGV("saveAddress is null, use origin address");
            address = option->getAddress();
        } else {
            address = option->getSaveAddress();
        }
        if (option->getScaleWidth() > 0 && option->getScaleHeight() > 0) {
            bool result = saveImg(address, saveImgData, option->getScaleWidth(),
                                  option->getScaleHeight(),
                                  option->srcChannel);
            if (result) {
                error = "";
            } else {
                error = "error saveImage fail";
            }
        } else {
            error = "error scaleImgWidth || scaleImgHeight = 0";
        }
    }

    if (listener != nullptr) {
        if (error.empty()) {
            listener->filterRenderComplete(option);
        } else {
            listener->filterRenderFail(option, error);
        }
    }
    return error;
}

bool FbFilter::saveImg(const string saveFileAddress,unsigned char* data,int width,int height, int channel) {
    if (data == nullptr) {
        HLOGE("saveImg data is null");
        return false;
    }

    //屏幕到文件保存需要使用
    stbi_flip_vertically_on_write(1);
    //保存图片到本地文件
//    if (channel == 3) {
//        if (stbi_write_jpg(saveFileAddress.c_str(), width, height, channel, data, 0)) {
//            HLOGV("save address = %s success", saveFileAddress.c_str());
////            free(data);
//            memset(saveImgData, 0, imgSize);
//            return true;
//        } else {
////            free(data);
//            memset(saveImgData, 0, imgSize);
//            HLOGE("save fail address = %s fail", saveFileAddress.c_str());
//            return false;
//        }
//    } else if (channel == 4) {
    if (stbi_write_png(saveFileAddress.c_str(), width, height, 4, data, 0)) {
        HLOGV("save address = %s success", saveFileAddress.c_str());
//            free(data);
        memset(saveImgData, 0, imgSize);
        return true;
    } else {
//            free(data);
        memset(saveImgData, 0, imgSize);
        HLOGE("save fail address = %s fail", saveFileAddress.c_str());
        return false;
    }
//    }
}

void FbFilter::destroyPixelBuffers() {
    if (pixelBuffer > 0) {
        HLOGV("destroyPixelBuffers");
        glDeleteTextures(1, &pixelBuffer);
    }
}

void FbFilter::readBuffer() {
    //加锁
//    std::unique_lock<std::mutex> lock(gMutex);
//
//    long size = scaleImgWidth * scaleImgHeight * 4;
//    saveImgData = (unsigned char *) malloc(sizeof(unsigned char)*size);
//    //清空数据
//    memset(saveImgData,0, sizeof(unsigned char)*size);
//
////    glReadBuffer(GL_FRONT);
//    //对齐像素字节
//    glPixelStorei(GL_PACK_ALIGNMENT, 1);
//    checkGLError("glPixelStorei");
//    //获取帧内字节
//    glReadPixels(0, 0, scaleImgWidth, scaleImgHeight, GL_RGBA, GL_UNSIGNED_BYTE, saveImgData);
}

void FbFilter::setListener(FilterListener *listener) {
    this->listener = listener;
}
