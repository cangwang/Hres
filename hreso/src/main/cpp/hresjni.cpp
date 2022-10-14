//
// Created by zengjiale on 2022/8/15.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window_jni.h>
#include <transform/hrestransformer.h>

#define LOG_TAG "HRESJNI"
#define ELOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ELOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#define HRES(sig) Java_com_cangwang_hreso_util_HresJniUtil_##sig

HresTransformer* hresTransformer;
jobject listener = nullptr;

extern "C" {

JNIEXPORT void JNICALL HRES(nativeCreateTransformer)(
        JNIEnv *env,
        jobject instance, jstring tag, jstring optionParams) {
    if (hresTransformer == nullptr) {
        hresTransformer = new HresTransformer();
    }
}

JNIEXPORT jobject JNICALL HRES(nativeTransformBitmap)(
        JNIEnv *env,
        jobject instance, jobject bitmap) {

}

JNIEXPORT jobject JNICALL HRES(nativeTransform)(
        JNIEnv *env,
        jobject instance, jstring address) {

}

JNIEXPORT void JNICALL HRES(nativeTransformAsync)(
        JNIEnv *env,
        jobject instance, jobject bitmap, jobject listener) {

}

JNIEXPORT void JNICALL HRES(nativeTransformRelease)(
        JNIEnv *env,
        jobject instance) {
    if (hresTransformer != nullptr) {
        hresTransformer->release();
        hresTransformer = nullptr;
    }
}

}
