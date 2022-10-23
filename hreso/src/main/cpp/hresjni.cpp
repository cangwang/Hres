//
// Created by zengjiale on 2022/8/15.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window_jni.h>
#include <transform/hrestransformer.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <util/stb_image_write.h>

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

JNIEXPORT jobject JNICALL HRES(nativeTransformBitmap)(
        JNIEnv *env,
        jobject instance, jobject bitmap) {
    HLOGV("nativeTransformBitmap");
}

JNIEXPORT jobject JNICALL HRES(nativeTransform)(
        JNIEnv *env,
        jobject instance, jstring address) {
    HLOGV("nativeTransform");
}

JNIEXPORT void JNICALL HRES(nativeTransformAsync)(
        JNIEnv *env,
        jobject instance, jobject bitmap, jobject listener) {
    HLOGV("nativeTransformAsync");
}

JNIEXPORT void JNICALL HRES(nativeTransformRelease)(
        JNIEnv *env,
        jobject instance) {
    if (hresTransformer != nullptr) {
        hresTransformer->release();
    }
}

}
