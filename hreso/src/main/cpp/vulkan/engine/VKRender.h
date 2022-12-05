//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKRENDER_H
#define HRES_VKRENDER_H

#include <vulkan_wrapper/vulkan_wrapper.h>
#include <vector>
#include <string>
#include <vulkan/Log.h>
#include <vulkan/engine/VKDeviceManager.h>
#include <vulkan/engine/VKSwapChainManager.h>
#include <vulkan/engine/VKBufferManager.h>
#include <vulkan/engine/VKOffScreen.h>
#include <vulkan/filter/VulkanFilter.h>
#include <vulkan/filter/OffScreenFilter.h>

using namespace std;
class VKRender {
public:
    VKRender();
    ~VKRender();

    int createRenderPass(VKDeviceManager *deviceInfo, VKSwapChainManager *swapChainInfo);

    int createCommandPool(VKDeviceManager *deviceInfo, VKSwapChainManager *swapChainInfo,
                          VKBufferManager *bufferInfo,
                          VKOffScreen *vkOffScreenInfo,
                          VulkanFilter * filter, VulkanFilter* offScreenFilter, VulkanFilter* effectFilter);

    int deleteCommandPool(VKDeviceManager *deviceInfo);

    void setImageLayout(VkCommandBuffer cmdBuffer,
                        VkImage image,
                        VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout,
                        VkPipelineStageFlags srcStages,
                        VkPipelineStageFlags destStages);

    VkRenderPass renderPass;
    VkCommandPool cmdPool;
    std::unique_ptr<VkCommandBuffer[]> cmdBuffer;
    uint32_t cmdBufferLen;
    VkSemaphore semaphore;
    VkFence fence;
};


#endif //HRES_VKRENDER_H
