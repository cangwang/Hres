//
// Created by cangwang on 2022/10/25.
//

#ifndef HRES_LISTENERMANAGER_H
#define HRES_LISTENERMANAGER_H


#include <jni.h>
#include <string>

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

private:
    JavaVM *javaVM;
    jobject listener;

    void callVoidMethod(const char *methodName, const char *sign, ...);

    bool attachCurrentThread(JNIEnv *&env, bool &isNeedDetach);
};


#endif //HRES_LISTENERMANAGER_H
