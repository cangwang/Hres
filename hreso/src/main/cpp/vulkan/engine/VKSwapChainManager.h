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
    int createFrameBuffer(VKDeviceManager* deviceInfo, VkRenderPass* renderPass, VkImageView depthView = VK_NULL_HANDLE);

    VkSwapchainKHR swapchain;
    uint32_t swapchainLength;

    VkExtent2D displaySize;
    VkExtent2D imageSize;
    VkFormat displayFormat;

    unique_ptr<VkFramebuffer[]> framebuffers;
    unique_ptr<VkImage[]> displayImages;
    unique_ptr<VkImageView[]> displayViews;
};


#endif //HRES_VKSWAPCHAINMANAGER_H
