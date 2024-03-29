
#include "eglcore.h"

#define LOG_TAG "EGLCore"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ELOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

/**
 * EGL是介于诸如OpenGL 或OpenVG的Khronos渲染API与底层本地平台窗口系统的接口。它被用于处理图形管理、表面/缓冲捆绑、渲染同步及支援使用其他Khronos API进行的高效、加速、混合模式2D和3D渲染。
 * cangwang 2018.12.1
 */
EGLCore::EGLCore():mDisplay(EGL_NO_DISPLAY),mSurface(EGL_NO_SURFACE),mContext(EGL_NO_CONTEXT) {

}

EGLCore::~EGLCore() {
    mDisplay = EGL_NO_DISPLAY;
    mSurface = EGL_NO_SURFACE;
    mContext = EGL_NO_CONTEXT;
}

void EGLCore::start(ANativeWindow* window, int width, int height) {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        HLOGE("eglGetDisplay failed: %d", eglGetError());
        return;
    }
    GLint majorVersion;
    GLint minorVersion;
    //获取支持最低和最高版本
    if (!eglInitialize(mDisplay, &majorVersion, &minorVersion)){
        HLOGE("eglInitialize failed: %d",eglGetError());
        return;
    }
    ELOGD("eglInitialize success");
    EGLConfig config = chooseConfig();
    ELOGD("chooseConfig success");

    mContext = createContext(mDisplay, config);
    ELOGD("createContext success");
    if (window) {
        EGLint format = 0;
        if (!eglGetConfigAttrib(mDisplay,config,EGL_NATIVE_VISUAL_ID,&format)){
            HLOGE("eglGetConfigAttrib failed: %d", eglGetError());
        }
        ELOGD("eglGetConfigAttrib success");
        ANativeWindow_setBuffersGeometry(window, 0, 0, format);
        ELOGD("setBuffersGeometry success");
    }
    //创建On-Screen 渲染区域
    if (window) {
        mSurface = eglCreateWindowSurface(mDisplay, config, window,0);
    } else {
        const EGLint pbuf_attribs[] = {
                EGL_WIDTH, width,
                EGL_HEIGHT, height,
                EGL_NONE};
        //创建离屏渲染Surface
        mSurface = eglCreatePbufferSurface(mDisplay, config, pbuf_attribs);
    }
    if (mSurface == nullptr || mSurface == EGL_NO_SURFACE){
        EGLint error = eglGetError();
        HLOGE("eglCreatePbufferSurface failed: %d", error);
        switch (error) {
            case EGL_BAD_ALLOC:
                HLOGE("Not enough resources available");
                break;
            case EGL_BAD_CONFIG:
                HLOGE("provided EGLConfig is invalid");
                break;
            case EGL_BAD_PARAMETER:
                HLOGE("provided EGL_WIDTH and EGL_HEIGHT is invalid");
                break;
            case EGL_BAD_MATCH:
                HLOGE("Check window and EGLConfig attributes");
                break;
        }
        return;
    }
    if (window) {
        ELOGD("eglCreateWindowSurface success");
    } else {
        ELOGD("eglCreatePbufferSurface success");
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        HLOGE("make current error:%d",eglGetError());
    }
    ELOGD("eglMakeCurrent success");
}

EGLConfig EGLCore::chooseConfig() {
    EGLint configsCount = 0;
    EGLConfig configs;
    EGLint attributes[] =
            { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
              EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
              EGL_RED_SIZE,   8,
              EGL_GREEN_SIZE, 8,
              EGL_BLUE_SIZE,  8,
              EGL_ALPHA_SIZE, 8,// if you need the alpha channel
              EGL_DEPTH_SIZE, 0,// if you need the depth buffer
              EGL_STENCIL_SIZE,0,
              EGL_NONE
            };
    int configSize = 1;
    if (eglChooseConfig(mDisplay, attributes, &configs, configSize, &configsCount)) {
        return configs;
    } else {
        HLOGE("eglChooseConfig failed: %d",eglGetError());
    }
    return nullptr;
}

EGLContext EGLCore::createContext(EGLDisplay eglDisplay, EGLConfig eglConfig) {
    //创建渲染上下文
    //只使用opengles2
    GLint contextAttrib[] = {EGL_CONTEXT_CLIENT_VERSION, 3 ,
                             EGL_NONE};
    // EGL_NO_CONTEXT表示不向其它的context共享资源
    mContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttrib);
    if (mContext == EGL_NO_CONTEXT) {
        EGLint error = eglGetError();
        HLOGE("eglCreateContext failed: %d", error);
        if (eglGetError() == EGL_BAD_CONFIG) {
            HLOGE("EGL_BAD_CONFIG");
        }
        return GL_FALSE;
    }
    return mContext;
}

GLboolean EGLCore::buildContext(ANativeWindow *window) {
    //与本地窗口通信
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY){
        HLOGE("eglGetDisplay failed: %d",eglGetError());
        return GL_FALSE;
    }

    GLint majorVersion;
    GLint minorVersion;
    //获取支持最低和最高版本
    if (!eglInitialize(mDisplay,&majorVersion,&minorVersion)){
        HLOGE("eglInitialize failed: %d",eglGetError());
        return GL_FALSE;
    }

    EGLConfig config;
    EGLint numConfigs = 0;
    //颜色使用565，读写类型需要egl扩展
    EGLint attribList[] = {
            EGL_RED_SIZE,5, //指定RGB中的R大小（bits）
            EGL_GREEN_SIZE,6, //指定G大小
            EGL_BLUE_SIZE,5,  //指定B大小
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR, //渲染类型，为相机扩展类型
            EGL_SURFACE_TYPE,EGL_WINDOW_BIT,  //绘图类型，
            EGL_NONE
    };

    //让EGL推荐匹配的EGLConfig
    if(!eglChooseConfig(mDisplay,attribList,&config,1,&numConfigs)){
        HLOGE("eglChooseConfig failed: %d",eglGetError());
        return GL_FALSE;
    }

    //找不到匹配的
    if (numConfigs <1){
        HLOGE("eglChooseConfig get config number less than one");
        return GL_FALSE;
    }

    //创建渲染上下文
    //只使用opengles3
    GLint contextAttrib[] = {EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    // EGL_NO_CONTEXT表示不向其它的context共享资源
    mContext = eglCreateContext(mDisplay,config,EGL_NO_CONTEXT,contextAttrib);
    if (mContext == EGL_NO_CONTEXT){
        HLOGE("eglCreateContext failed: %d",eglGetError());
        return GL_FALSE;
    }

    EGLint format = 0;
    if (!eglGetConfigAttrib(mDisplay,config,EGL_NATIVE_VISUAL_ID,&format)){
        HLOGE("eglGetConfigAttrib failed: %d",eglGetError());
        return GL_FALSE;
    }
    ANativeWindow_setBuffersGeometry(window,0,0,format);

    //创建On-Screen 渲染区域
    mSurface = eglCreateWindowSurface(mDisplay,config,window,0);
    if (mSurface == EGL_NO_SURFACE){
        HLOGE("eglCreateWindowSurface failed: %d",eglGetError());
        return GL_FALSE;
    }

    //把EGLContext和EGLSurface关联起来，单缓冲只使用了一个surface
    if (!eglMakeCurrent(mDisplay,mSurface,mSurface,mContext)){
        HLOGE("eglMakeCurrent failed: %d",eglGetError());
        return GL_FALSE;
    }

    ELOGD("buildContext Succeed");
    return GL_TRUE;
}

/**
 * 现在只使用单缓冲绘制
 */
void EGLCore::swapBuffer() {
    //双缓冲绘图，原来是检测出前台display和后台缓冲的差别的dirty区域，然后再区域替换buffer
    //1）首先计算非dirty区域，然后将非dirty区域数据从上一个buffer拷贝到当前buffer；
    //2）完成buffer内容的填充，然后将previousBuffer指向buffer，同时queue buffer。
    //3）Dequeue一块新的buffer，并等待fence。如果等待超时，就将buffer cancel掉。
    //4）按需重新计算buffer
    //5）Lock buffer，这样就实现page flip，也就是swapbuffer
    if (mDisplay != nullptr && mSurface != nullptr) {
        eglSwapBuffers(mDisplay, mSurface);
    }
}

void EGLCore::release() {
    eglDestroySurface(mDisplay,mSurface);
    eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
    eglDestroyContext(mDisplay,mContext);
    eglReleaseThread();
    eglTerminate(mDisplay);
    mDisplay = EGL_NO_DISPLAY;
    mSurface = EGL_NO_SURFACE;
    mContext = EGL_NO_CONTEXT;
}