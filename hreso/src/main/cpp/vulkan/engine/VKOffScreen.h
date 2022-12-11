//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKOFFSCREEN_H
#define HRES_VKOFFSCREEN_H

#include <vulkan_wrapper/vulkan_wrapper.h>
#include <vulkan/engine/VKDeviceManager.h>
#include <vulkan/engine/VKSwapChainManager.h>
#include <vulkan/engine/VKBufferManager.h>
#include <vulkan/engine/VKTextureManager.h>
#include <vector>

using namespace std;
class VKOffScreen {
public:
    struct FrameBufferAttachment {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    };

    struct OffscreenPass {
        int32_t width, height;
        vector<VkFramebuffer> frameBuffer;
        vector<FrameBufferAttachment> color;
        vector<VkSampler> sampler;
        //ImageView sampler, imageLayout
        vector<VkDescriptorImageInfo> descriptor;
        VkRenderPass renderPass;
    } offscreenPass;

    struct VulkanGfxPipelineInfo {
        VkDescriptorSetLayout dscLayout_;
        VkDescriptorPool descPool_;
        VkDescriptorSet descSet_;
        VkPipelineLayout layout_;
        VkPipelineCache cache_;
        VkPipeline pipeline_;
    };

    VulkanGfxPipelineInfo offscreenPipeline;

    void createOffscreen(VKDeviceManager*deviceInfo, VKSwapChainManager *swapChainInfo);

    VkImage createOffscreen(VKDeviceManager*deviceInfo, VkFormat format, int width, int height);

    int updateOffScreenDescriptorSet(VKDeviceManager *deviceInfo, VKBufferManager *bufferInfo, VKTextureManager *textureInfo);


    VkResult allocateMemoryTypeFromProperties(VKDeviceManager *deviceInfo, uint32_t typeBits,
                                              VkFlags requirements_mask,
                                              uint32_t *typeIndex);

    int num = 1;
};


#endif //HRES_VKOFFSCREEN_H
