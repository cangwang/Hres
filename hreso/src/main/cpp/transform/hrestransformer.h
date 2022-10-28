//
// Created by cangwang on 2022/10/14.
//

#ifndef HRES_HRESTRANSFORMER_H
#define HRES_HRESTRANSFORMER_H

#include <android/log.h>
#include <string>
#include <deque>
#include <transform/ioptions.h>
#include <transform/imagehrestransformer.h>
#include <egl/eglcore.h>
#include <transform/optionparser.h>
#include <transform/listenermanager.h>
#include <jni.h>

#define LOG_TAG "HresTransformer"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class HresTransformer: FilterListener {
public:
    HresTransformer();
    ~HresTransformer();
    void addOption(string options, jobject op);
    void transform();
    void release();
    void setListener(ListenerManager* listenerManager);

private:
    void filterRenderComplete(IOptions *options) override;

    void filterRenderFail(IOptions *options, string errorMsg) override;

private:
    shared_ptr<OptionParser> optionParser;
    shared_ptr<deque<IOptions*>> optionsList;
    shared_ptr<ImageHresTransformer> imageHresTransformer;
    ListenerManager* listenerManager;
    bool removeOptions(string address);
    bool isTransforming = false;
};


#endif //HRES_HRESTRANSFORMER_H
