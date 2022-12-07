//
// Created by asus on 2022/11/7.
//

#include "VideoRenderContext.h"

VideoRenderContext::jni_fields_t VideoRenderContext::jni_fields = { 0L };

VideoRenderContext::VideoRenderContext(int type) {
    m_pVideoRenderer = VideoRenderer::create(type);
}

VideoRenderContext::~VideoRenderContext() {

}

void VideoRenderContext::init(ANativeWindow *window, size_t width, size_t height,
                              AAssetManager *manager) {
    m_pVideoRenderer->init(window, width, height, manager);
}

void VideoRenderContext::render() {
    m_pVideoRenderer->render();
}

void VideoRenderContext::draw(uint8_t *buffer, size_t length, size_t width, size_t height,
                              int rotation) {
    m_pVideoRenderer->draw(buffer,length, width, height, rotation);
}

void VideoRenderContext::setParameters(uint32_t params) {
    m_pVideoRenderer->setParameters(params);
}

void VideoRenderContext::setProcess(uint32_t params) {

}

uint32_t VideoRenderContext::getParameters() {
    return m_pVideoRenderer->getParameters();
}

void VideoRenderContext::createContext(JNIEnv *env, jobject obj, jint type) {
    VideoRenderContext* context = new VideoRenderContext(type);

    storeContext(env, obj, context);
}

void VideoRenderContext::storeContext(JNIEnv *env, jobject obj, VideoRenderContext *context) {
// Get a reference to this object's class
    jclass cls = env->GetObjectClass(obj);

    if (NULL == cls)
    {
        HLOGE("Could not find com/media/camera/preview/render/VideoRenderer.");
        return;
    }

    // Get the Field ID of the "mNativeContext" variable
    jni_fields.context = env->GetFieldID(cls, "mNativeContext", "J");
    if (NULL == jni_fields.context)
    {
        HLOGE("Could not find mNativeContext.");
        return;
    }

    env->SetLongField(obj, jni_fields.context, (jlong)context);
}

void VideoRenderContext::deleteContext(JNIEnv *env, jobject obj) {
    if (NULL == jni_fields.context)
    {
        HLOGE("Could not find mNativeContext.");
        return;
    }

    VideoRenderContext* context = reinterpret_cast<VideoRenderContext*>(env->GetLongField(obj, jni_fields.context));

    if (context) delete context;

    env->SetLongField(obj, jni_fields.context, 0L);
}

VideoRenderContext *VideoRenderContext::getContext(JNIEnv *env, jobject obj) {
    if (NULL == jni_fields.context)
    {
        HLOGE("Could not find mNativeContext.");
        return NULL;
    }

    VideoRenderContext* context = reinterpret_cast<VideoRenderContext*>(env->GetLongField(obj, jni_fields.context));

    return context;
}
