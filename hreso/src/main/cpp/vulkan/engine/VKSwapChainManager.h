//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKSWAPCHAINMANAGER_H
#define HRES_VKSWAPCHAINMANAGER_H


#include <vulkan/engine/VKDeviceManager.h>


using namespace std;
class VKSwapChainManager {
public:
    VKSwapChainManager();
    ~VKSwapChainManager();

    int createSwapChain(VKDeviceManager* info);
    int createSwapChain(VKDeviceManager* info, int width, int height);
    int createOffscreen(VkFormat format, int width, int height);
    int createFrameBuffer(VKDeviceManager* deviceInfo, VkRenderPass* renderPass, VkImageView depthView = VK_NULL_HANDLE);
    int createOffscreenFrameBuffer(VKDeviceManager *deviceInfo, VkRenderPass* renderPass,
                                                       VkImageView depthView);
    VkResult allocateMemoryTypeFromProperties(VKDeviceManager *deviceInfo, uint32_t typeBits,
                                                                  VkFlags requirements_mask, uint32_t *typeIndex);

    VkSwapchainKHR swapchain;
    uint32_t swapchainLength;

    VkExtent2D displaySize;
    VkExtent2D imageSize;
    VkFormat displayFormat;

    unique_ptr<VkFramebuffer[]> framebuffers;
    unique_ptr<VkImage[]> displayImages;
    unique_ptr<VkImageView[]> displayViews;
    unique_ptr<VkDeviceMemory[]> displayMem;
    VkImage* lastDisplayImage;
};


#endif //HRES_VKSWAPCHAINMANAGER_H
