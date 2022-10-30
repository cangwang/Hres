//
// Created by cangwang on 2022/10/25.
//

#ifndef HRES_LISTENERMANAGER_H
#define HRES_LISTENERMANAGER_H

#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "ListenerManager"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class ListenerManager {
public:
    ListenerManager(JavaVM *javaVM_) : javaVM(javaVM_) {};

    ~ListenerManager();

    void hresTransformStart(jobject option);

    void hresTransformComplete(jobject option);

    void hresTransformError(jobject option, string errorTag);

    void setListener(jobject &_listener) {
        listener = _listener;
    };

    void setEnv(JNIEnv* _env) {
        env = _env;
    };

private:
    JavaVM *javaVM;
    jobject listener;
    JNIEnv* env;

    void callVoidMethod(const char *methodName, const char *sign, ...);

    bool attachCurrentThread(JNIEnv *&env, bool &isNeedDetach);
};


#endif //HRES_LISTENERMANAGER_H
