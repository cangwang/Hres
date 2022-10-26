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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <util/stb_image_write.h>
#include <transform/imageoptionparams.h>

#define LOG_TAG "HRESJNI"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#define HRES(sig) Java_com_cangwang_hreso_util_HresJniUtil_##sig

unique_ptr<HresTransformer> hresTransformer;
jobject listener = nullptr;

extern "C" {

JNIEXPORT void JNICALL HRES(nativeCreateTransformer)(
        JNIEnv *env,
        jobject instance, jstring tag, jstring optionParams) {
    HLOGV("nativeCreateTransformer");
    if (hresTransformer == nullptr) {
        hresTransformer = make_unique<HresTransformer>();
    }
}

JNIEXPORT void JNICALL HRES(nativeTransform) (
        JNIEnv *env,
        jobject instance, jstring options) {
    HLOGV("nativeTransform");
    if (hresTransformer) {
        const char* optionsStr = env->GetStringUTFChars(options, JNI_FALSE);
        hresTransformer->addOption(string(optionsStr));
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
        if (listener) {
            env->DeleteGlobalRef(listener);
        }
        listener = env->NewGlobalRef(listener);

        ListenerManager* listenerManager = new ListenerManager(javaVm);

        listenerManager->setListener(listener);
        hresTransformer->setListener(listenerManager);
    }
}

JNIEXPORT void JNICALL HRES(nativeTransformAsync)(
        JNIEnv *env,
        jobject instance, jstring options) {
    HLOGV("nativeTransformAsync");
    if (hresTransformer) {
        const char* optionsStr = env->GetStringUTFChars(options, JNI_FALSE);
        hresTransformer->addOption(string(optionsStr));
        env->ReleaseStringUTFChars(options, optionsStr);
    }
}

JNIEXPORT void JNICALL HRES(nativeTransformRelease)(
        JNIEnv *env,
        jobject instance) {
    if (hresTransformer != nullptr) {
        hresTransformer->release();
        hresTransformer = nullptr;
    }

    if (listener) {
        env->DeleteGlobalRef(listener);
        listener = nullptr;
    }
}

}
