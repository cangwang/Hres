//
// Created by asus on 2022/11/8.
//

#include "VKSwapChainManager.h"

VKSwapChainManager::VKSwapChainManager() {

}

VKSwapChainManager::~VKSwapChainManager() {
    lastDisplayImage = nullptr;
}

int VKSwapChainManager::createSwapChain(VKDeviceManager *info) {
    //查询交换链支持情况
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info->physicalDevice, info->surface, &surfaceCapabilities);
    //查询支持Surface格式
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(info->physicalDevice, info->surface, &formatCount, nullptr);

    unique_ptr<VkSurfaceFormatKHR[]> formats = make_unique<VkSurfaceFormatKHR[]>(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(info->physicalDevice, info->surface, &formatCount, formats.get());

    //RGBA格式
    uint32_t chosenFormat;
    for (int i = 0; i <formatCount; ++i) {
        if (formats[i].format ==  VK_FORMAT_R8G8B8A8_UNORM) {
            chosenFormat = i;
            break;
        }
    }
    displaySize = surfaceCapabilities.currentExtent;
    displayFormat = formats[chosenFormat].format;
    //创建交换链
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

int VKSwapChainManager::createSwapChain(VKDeviceManager *info, int width, int height) {
    //查询交换链支持情况
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info->physicalDevice, info->surface, &surfaceCapabilities);
    //查询支持Surface格式
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(info->physicalDevice, info->surface, &formatCount, nullptr);

    unique_ptr<VkSurfaceFormatKHR[]> formats = make_unique<VkSurfaceFormatKHR[]>(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(info->physicalDevice, info->surface, &formatCount, formats.get());

    //RGBA格式
    uint32_t chosenFormat;
    for (int i = 0; i <formatCount; ++i) {
        // RGBA格式
        if (formats[i].format ==  VK_FORMAT_R8G8B8A8_UNORM) {
            chosenFormat = i;
            break;
        }
    }
    //屏幕大小
    displaySize = surfaceCapabilities.currentExtent;
    //图片大小
    imageSize = VkExtent2D();
    imageSize.width = width;
    imageSize.height = height;
    displayFormat = formats[chosenFormat].format;
    //创建交换链
    VkSwapchainCreateInfoKHR swapchainCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = info->surface,
            //交换链实际纹理图片数量，实际缓存解析的纹理图片数量
            .minImageCount = 1,
            .imageFormat = formats[chosenFormat].format,
            .imageColorSpace = formats[chosenFormat].colorSpace,
            // 不能填屏幕大小，不然图片大小超出屏幕会有绘制错位问题
            // copyImage的时候,交换链、帧缓冲Framebuffer以及视口VKViewport都需要设置为图片大小
            .imageExtent = imageSize,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .imageArrayLayers = 1,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &info->queueFamilyIndex,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR,
            .oldSwapchain = VK_NULL_HANDLE,
            .clipped = VK_TRUE
    };

    CALL_VK(vkCreateSwapchainKHR(info->device, &swapchainCreateInfo, nullptr, &swapchain))
    CALL_VK(vkGetSwapchainImagesKHR(info->device, swapchain, &swapchainLength, nullptr))

    return 0;
}

int VKSwapChainManager::createFrameBuffer(VKDeviceManager *deviceInfo, VkRenderPass* renderPass,
                                          VkImageView depthView) {
    //图像被交换链创建，也会在交换链销毁的同时自动清理，所以我们不需要添加任何清理代码
    //首先通过调用vkGetSwapchainImagesKHR获取交换链中图像的数量，并根据数量设置合适的容器大小保存获取到的句柄集合
    uint32_t swapChanImageCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(deviceInfo->device, swapchain, &swapChanImageCount, nullptr))
    //创建Image
    displayImages = make_unique<VkImage[]>(swapChanImageCount);
    CALL_VK(vkGetSwapchainImagesKHR(deviceInfo->device, swapchain, &swapChanImageCount, displayImages.get()))

    //https://blog.csdn.net/qq_35312463/article/details/103916596?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166977539016782429758588%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=166977539016782429758588&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-19-103916596-null-null.nonecase&utm_term=vulkan%E5%A1%AB%E5%9D%91%E5%AD%A6%E4%B9%A0day&spm=1018.2226.3001.4450
    //每一个交换链中的图像创建基本的视图
    displayViews = make_unique<VkImageView[]>(swapChanImageCount);
    for (int i = 0; i < swapChanImageCount; ++i) {
        VkImageViewCreateInfo viewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = displayImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D, // 2D纹理
            .format = displayFormat,
            //components字段允许调整颜色通道的最终的映射逻辑
            .components = {
                    .r = VK_COMPONENT_SWIZZLE_R,
                    .g = VK_COMPONENT_SWIZZLE_G,
                    .b = VK_COMPONENT_SWIZZLE_B,
                    .a = VK_COMPONENT_SWIZZLE_A,
            },
            //subresourceRangle字段用于描述图像的使用目标是什么，以及可以被访问的有效区域
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
            .width = static_cast<uint32_t>(imageSize.width),
            .height = static_cast<uint32_t>(imageSize.height)
        };

        fbCreateInfo.attachmentCount = (depthView == VK_NULL_HANDLE ? 1 : 2);

        CALL_VK(vkCreateFramebuffer(deviceInfo->device, &fbCreateInfo, nullptr, &framebuffers[i]))
    }

    lastDisplayImage = &displayImages[0];
    return 0;
}