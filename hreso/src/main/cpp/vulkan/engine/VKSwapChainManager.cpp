//
// Created by asus on 2022/11/8.
//

#include "VKSwapChainManager.h"

VKSwapChainManager::VKSwapChainManager() {

}

VKSwapChainManager::~VKSwapChainManager() {

}

int VKSwapChainManager::createSwapChain(VKDeviceManager *info) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info->physicalDevice, info->surface, &surfaceCapabilities);
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(info->physicalDevice, info->surface, &formatCount, nullptr);

    unique_ptr<VkSurfaceFormatKHR[]> formats = make_unique<VkSurfaceFormatKHR[]>(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(info->physicalDevice, info->surface, &formatCount, formats.get());

    uint32_t chosenFormat;
    for (int i = 0; i <formatCount; ++i) {
        if (formats[i].format ==  VK_FORMAT_R8G8B8A8_UNORM) {
            chosenFormat = i;
            break;
        }
    }
    displaySize = surfaceCapabilities.currentExtent;
    displayFormat = formats[chosenFormat].format;

    VkSwapchainCreateInfoKHR swapchainCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .surface = info->surface,
        .minImageCount = surfaceCapabilities.minImageCount,
        .imageFormat = formats[chosenFormat].format,
        .imageColorSpace = formats[chosenFormat].colorSpace,
        .imageExtent = surfaceCapabilities.currentExtent,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .imageArrayLayers = 1,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &info->queueFamilyIndex,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .oldSwapchain = VK_NULL_HANDLE,
        .clipped = VK_FALSE
    };

    CALL_VK(vkCreateSwapchainKHR(info->device, &swapchainCreateInfo, nullptr, &swapchain))
    CALL_VK(vkGetSwapchainImagesKHR(info->device, swapchain, &swapchainLength, nullptr))

    return 0;
}

int VKSwapChainManager::createFrameBuffer(VKDeviceManager *deviceInfo, VkRenderPass* renderPass,
                                          VkImageView depthView) {
    uint32_t swapChanImageCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(deviceInfo->device, swapchain, &swapChanImageCount, nullptr))
    //创建Image
    displayImages = make_unique<VkImage[]>(swapChanImageCount);
    CALL_VK(vkGetSwapchainImagesKHR(deviceInfo->device, swapchain, &swapChanImageCount, displayImages.get()))

    displayViews = make_unique<VkImageView[]>(swapChanImageCount);
    for (int i = 0; i < swapChanImageCount; ++i) {
        VkImageViewCreateInfo viewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = displayImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = displayFormat,
            .components = {
                    .r = VK_COMPONENT_SWIZZLE_R,
                    .g = VK_COMPONENT_SWIZZLE_G,
                    .b = VK_COMPONENT_SWIZZLE_B,
                    .a = VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
            }
        };
        //创建显示窗口
        CALL_VK(vkCreateImageView(deviceInfo->device, &viewCreateInfo, nullptr, &displayViews[i]))
    }

    //对每个交换Image创建帧缓冲
    framebuffers = make_unique<VkFramebuffer[]>(swapChanImageCount);
    for (int i = 0; i < swapchainLength; ++i) {
        VkImageView attachments[2] {
            displayViews[i],
            depthView
        };
        VkFramebufferCreateInfo fbCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = *renderPass,
            .layers = 1,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = static_cast<uint32_t>(displaySize.width),
            .height = static_cast<uint32_t>(displaySize.height)
        };

        fbCreateInfo.attachmentCount = (depthView == VK_NULL_HANDLE ? 1 : 2);

        CALL_VK(vkCreateFramebuffer(deviceInfo->device, &fbCreateInfo, nullptr, &framebuffers[i]))
    }

    return 0;
}
