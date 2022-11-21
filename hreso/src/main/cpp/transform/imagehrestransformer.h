//
// Created by cangwang on 2022/10/14.
//

#ifndef HRES_IMAGEHRESTRANSFORMER_H
#define HRES_IMAGEHRESTRANSFORMER_H

#include <src/main/cpp/bean/ioptions.h>
#include <list>
#include <filter/filtercontroller.h>
#include <filter/filterlistener.h>
#include <transform/HresBaseTransformer.h>

#define LOG_TAG "ImageHresTransformer"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
class ImageHresTransformer : public HresBaseTransformer {
public:
    ImageHresTransformer();
    ~ImageHresTransformer();
    void setWindow(ANativeWindow* window) override;
    void updateViewPoint(int width, int height) override;
    void transformOption(IOptions* options) override;
    void setListener(FilterListener* listener) override;
    void transform() override;
    void release() override;

private:
    shared_ptr<FilterController> filterController;

    shared_ptr<IOptions> option;
};

#endif //HRES_IMAGEHRESTRANSFORMER_H
