//
// Created by asus on 2022/11/7.
//

#ifndef HRES_VIDEORENDERCONTEXT_H
#define HRES_VIDEORENDERCONTEXT_H

#include <memory>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <vulkan/VideoRenderer.h>
#include <android/log.h>

#define LOG_TAG "VideoRenderContext"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class VideoRenderContext {
public:
    struct jni_fields_t {
        jfieldID context;
    };

    VideoRenderContext(int type);
    ~VideoRenderContext();

    void init(ANativeWindow* window, size_t width, size_t height, AAssetManager* manager);
    void initWindow(ANativeWindow *window, size_t width, size_t height);
    void render();
    void draw(uint8_t * buffer, size_t length, size_t width, size_t height, int rotation);
    void setParameters(uint32_t params);
    void setProcess(uint32_t params);
    uint32_t getParameters();

    static void createContext(JNIEnv* env, jobject obj, jint type);
    static void storeContext(JNIEnv* env, jobject obj, VideoRenderContext *context);
    static void deleteContext(JNIEnv* env, jobject obj);
    static VideoRenderContext* getContext(JNIEnv* env, jobject object);

    AAssetManager* manager;

private:
    unique_ptr<VideoRenderer> m_pVideoRenderer;
    static jni_fields_t jni_fields;
};


#endif //HRES_VIDEORENDERCONTEXT_H
