//
// Created by cangwang on 2022/10/25.
//

#include "listenermanager.h"

ListenerManager::~ListenerManager() {
    listener = nullptr;
    javaVM = nullptr;
}

void ListenerManager::hresTransformStart(jobject option) {
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jclass clazz_listener = env->GetObjectClass(listener);
    jmethodID methodId = env->GetMethodID(clazz_listener, "hresTransformStart",
                                          "(Lcom/cangwang/hreso/bean/OptionParams;)V");
    env->CallVoidMethod(listener, methodId, option);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}

void ListenerManager::hresTransformComplete(jobject option) {
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jclass clazz_listener = env->GetObjectClass(listener);
    jmethodID methodId = env->GetMethodID(clazz_listener, "hresTransformComplete",
                                          "(Lcom/cangwang/hreso/bean/OptionParams;)V");
    env->CallVoidMethod(listener, methodId, option);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}

void ListenerManager::hresTransformError(jobject option, string errorMsg) {
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jstring jmsg = env->NewStringUTF(errorMsg.c_str());
    jclass clazz_listener = env->GetObjectClass(listener);
    jmethodID methodId = env->GetMethodID(clazz_listener, "hresTransformError", "(Lcom/cangwang/hreso/bean/OptionParams;Ljava/lang/String;)V");
    env->CallVoidMethod(listener, methodId, option, jmsg);
    env->DeleteLocalRef(jmsg);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}

void ListenerManager::callVoidMethod(const char *methodName, const char *sign, ...) {
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jclass clazz_listener = env->GetObjectClass(listener);
    jmethodID methodId = env->GetMethodID(clazz_listener, methodName, sign);
    va_list ap;
    va_start(ap, sign);
    env->CallVoidMethodV(listener, methodId, ap);
    va_end(ap);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}

bool ListenerManager::attachCurrentThread(JNIEnv *&env, bool &isNeedDetach) {
    if (!javaVM) {
        return false;
    }
    isNeedDetach = false;
//    ELOGV("EvaListenerManager::attachCurrentThread");
    jint res = javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = javaVM->AttachCurrentThread(&env, nullptr);
        if (JNI_OK != res) {
            return false;
        }
        isNeedDetach = true;
    }
    return true;
}
