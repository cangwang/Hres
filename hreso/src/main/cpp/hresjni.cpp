//
// Created by cangwang on 2022/8/15.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window_jni.h>
#include <transform/imageoptionparams.h>
#include <transform/hrestransformer.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <util/stb_image.h>
#include <util/stb_image_write.h>
#include <transform/imageoptionparams.h>

#define LOG_TAG "HRESJNI"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#define HRES(sig) Java_com_cangwang_hreso_util_HresJniUtil_##sig

unique_ptr<HresTransformer> hresTransformer;
jobject l = nullptr;

extern "C" {

JNIEXPORT void JNICALL HRES(nativeCreateTransformer) (
        JNIEnv *env,
        jobject instance, jstring tag, jstring optionParams, jobject surface) {
    HLOGV("nativeCreateTransformer");
    if (hresTransformer == nullptr) {
        hresTransformer = make_unique<HresTransformer>();
        if (surface != nullptr) {
            //创建window
            ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
            if (window == nullptr) {
                ELOGE("window is nullptr");
            } else {
                hresTransformer->setWindow(window);
            }
        }
    }
}

JNIEXPORT void JNICALL HRES(nativeUpdateViewPoint) (
        JNIEnv *env,
        jobject instance, jint width, jint height) {
    HLOGV("nativeCreateTransformer");
    if (hresTransformer != nullptr) {
        hresTransformer->updateViewPoint(width, height);
    }
}

JNIEXPORT void JNICALL HRES(nativeTransform) (
        JNIEnv *env,
        jobject instance, jstring options, jobject op) {
    HLOGV("nativeTransform");
    if (hresTransformer) {
        const char* optionsStr = env->GetStringUTFChars(options, JNI_FALSE);
        op = env->NewGlobalRef(op);
        hresTransformer->addOption(string(optionsStr), op);
        env->ReleaseStringUTFChars(options, optionsStr);
    }
}

JNIEXPORT void JNICALL HRES(nativeSetListener)(
        JNIEnv *env,
        jobject instance, jobject listener) {
    HLOGV("nativeSetListener");
    if (hresTransformer != nullptr && listener != nullptr) {
        JavaVM *javaVm;
        if (env->GetJavaVM(&javaVm) != JNI_OK) {
            return;
        }
        if (l) {
            env->DeleteGlobalRef(l);
        }
        l = env->NewGlobalRef(listener);

        ListenerManager* listenerManager = new ListenerManager(javaVm);
        listenerManager->setEnv(env);

        listenerManager->setListener(l);
        hresTransformer->setListener(listenerManager);
    }
}

JNIEXPORT void JNICALL HRES(nativeTransformRelease)(
        JNIEnv *env,
        jobject instance) {
    if (hresTransformer != nullptr) {
        hresTransformer->release();
        hresTransformer = nullptr;
    }

    if (l) {
        env->DeleteGlobalRef(l);
        l = nullptr;
    }
}

}
