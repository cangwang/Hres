//
// Created by asus on 2022/11/12.
//

#ifndef HRES_OFFSCREENFILTER_H
#define HRES_OFFSCREENFILTER_H

#include <vulkan/filter/VulkanFilter.h>

using namespace std;
class OffScreenFilter : public VulkanFilter {
public:
    OffScreenFilter(): VulkanFilter() {
        pVertexShader = kVertexShader;
        pFragShader = kFragmentShader;
    }

    virtual int init(VkDevice device, VkRenderPass renderPass) override;

    int updateDescriptorSet(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout) override;

    virtual int updateDescriptorSet(vector<VkDescriptorBufferInfo>& bufferInfo, vector<VkDescriptorImageInfo>& imageInfo);

private:
    virtual int createDescriptorLayout() override;
    virtual int createDescriptorSet() override;
};


#endif //HRES_OFFSCREENFILTER_H
