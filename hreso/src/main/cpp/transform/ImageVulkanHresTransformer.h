//
// Created by asus on 2022/11/17.
//

#ifndef HRES_IMAGEVULKANHRESTRANSFORMER_H
#define HRES_IMAGEVULKANHRESTRANSFORMER_H

#include <transform/HresBaseTransformer.h>
#include <android/native_window.h>
#include <vulkan/engine/VKEngineRenderer.h>
#include <util/loadtextureutil.h>

using namespace std;
class ImageVulkanHresTransformer: public HresBaseTransformer {
public:
    ImageVulkanHresTransformer();
    ~ImageVulkanHresTransformer();
    void setWindow(ANativeWindow* window);
    void updateViewPoint(int width, int height);
    void transformOption(IOptions* options);
    void setListener(FilterListener* listener);
    void transform() override;
    void release() override;

private:
    shared_ptr<VKEngineRenderer> engine;
    ANativeWindow* window;
    size_t width;
    size_t height;
    unsigned char* image;
    IOptions* options;
};


#endif //HRES_IMAGEVULKANHRESTRANSFORMER_H
