//
// Created by asus on 2022/11/8.
//

#include "VKTextureManager.h"

VkResult VKTextureManager::LoadTextureFromFile(VKDeviceManager *deviceInfo, AAssetManager *manager,
                                               const char *filePath,
                                               VKTextureManager::texture_object *tex_obj,
                                               VkImageUsageFlags usage, VkFlags required_props) {
    if (!(usage | required_props)) {
        HLOGE("tutorial texture, No usage and required_pros");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    VkFormatProperties props;
    bool needBlit = true;

    vkGetPhysicalDeviceFormatProperties(deviceInfo->physicalDevice, kTexFmt, &props);
    //线性平铺和最优化平铺
    assert((props.linearTilingFeatures | props.optimalTilingFeatures)
            & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    //支持线性平铺
    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        needBlit = false;
    }

    AAsset* file = AAssetManager_open(manager, filePath, AASSET_MODE_BUFFER);
    ssize_t fileLength = AAsset_getLength(file);
    stbi_uc* fileContent = new unsigned char[fileLength];
    AAsset_read(file, fileContent, fileLength);
    AAsset_close(file);

    uint32_t imgWidth, imgHeight, n;
    unsigned char* imageData = stbi_load_from_memory(fileContent, fileLength,
                                                   reinterpret_cast<int*>(&imgWidth),
                                                   reinterpret_cast<int*>(&imgHeight),
                                                   reinterpret_cast<int*>(&n), 4);
    assert(n == 4);

    tex_obj->tex_width = imgWidth;
    tex_obj->tex_height = imgHeight;

    VkImageCreateInfo image_create_info {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D, //2D图片
        .format = kTexFmt,
        .extent = {static_cast<uint32_t>(imgWidth),
                   static_cast<uint32_t>(imgHeight), 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_LINEAR,
        //线性平铺用源，
        .usage = (needBlit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &deviceInfo->queueFamilyIndex,
        .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
    };

    VkMemoryAllocateInfo mem_alloc {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = 0,
        .memoryTypeIndex = 0
    };

    VkMemoryRequirements mem_reqs;
    // 创建图片
    CALL_VK(vkCreateImage(deviceInfo->device, & image_create_info, nullptr, &tex_obj->image))
    //测量图片大小
    vkGetImageMemoryRequirements(deviceInfo->device, tex_obj->image, &mem_reqs);
    //设置内存大小
    mem_alloc.allocationSize = mem_reqs.size;

    VK_CHECK(allocateMemoryTypeFromProperties(deviceInfo, mem_reqs.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                              &mem_alloc.memoryTypeIndex))

    CALL_VK(vkAllocateMemory(deviceInfo->device, &mem_alloc, nullptr, &tex_obj->mem))
    CALL_VK(vkBindImageMemory(deviceInfo->device, tex_obj->image, tex_obj->mem, 0))

    //能否被CPU访问
    if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        //找到 subresource 图像数据的起始 offset,并进行映射
        const VkImageSubresource subres {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .arrayLayer = 0
        };
        VkSubresourceLayout layout;
        void* data;
        vkGetImageSubresourceLayout(deviceInfo->device, tex_obj->image, &subres, &layout);
        CALL_VK(vkMapMemory(deviceInfo->device, tex_obj->mem, 0, mem_alloc.allocationSize, 0, &data))
        // Todo 读取出图像数据
        for (int32_t y = 0; y < imgHeight; ++y) {
            unsigned char* row = (unsigned char*)((char*)data + layout.rowPitch * y);
            for (int32_t x = 0; x < imgWidth; x++) {
                row[x * 4] = imageData[(x + y * imgWidth) * 4];
                row[x * 4 + 1] = imageData[(x + y * imgWidth) * 4 + 1];
                row[x * 4 + 2] = imageData[(x + y * imgWidth) * 4 + 2];
                row[x * 4 + 3] = imageData[(x + y * imgWidth) * 4 + 3];
            }
        }
        vkUnmapMemory(deviceInfo->device, tex_obj->mem);
        stbi_image_free(imageData);
    }
    delete[] fileContent;

    tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    //创建命令池
    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = deviceInfo->queueFamilyIndex,
    };
    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(deviceInfo->device, &cmdPoolCreateInfo, nullptr,
                                &cmdPool));

    VkCommandBuffer gfxCmd;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    CALL_VK(vkAllocateCommandBuffers(deviceInfo->device, &cmd, &gfxCmd));
    VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd, &cmd_buf_info))

    VkImage stageImage = VK_NULL_HANDLE;
    VkDeviceMemory stageMem = VK_NULL_HANDLE;
    if (!needBlit) { //非平铺
        setImageLayout(gfxCmd, tex_obj->image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    } else {
        //放到临时存储
        stageImage = tex_obj->image;
        stageMem = tex_obj->mem;
        tex_obj->image = VK_NULL_HANDLE;
        tex_obj->mem = VK_NULL_HANDLE;
        //复用原图像结构体
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //创建目标图片和绑定内存
        CALL_VK(vkCreateImage(deviceInfo->device, &image_create_info, nullptr, &tex_obj->image))
        //获取图片需要内存大小
        vkGetImageMemoryRequirements(deviceInfo->device, tex_obj->image, &mem_reqs);

        mem_alloc.allocationSize = mem_reqs.size;

        VK_CHECK(allocateMemoryTypeFromProperties(deviceInfo, mem_reqs.memoryTypeBits,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex))
        CALL_VK(vkAllocateMemory(deviceInfo->device, &mem_alloc, nullptr, &tex_obj->mem))
        CALL_VK(vkBindImageMemory(deviceInfo->device, tex_obj->image, tex_obj->mem, 0))

        setImageLayout(gfxCmd, stageImage, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        setImageLayout(gfxCmd, tex_obj->image, VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        //复制指定区域
        VkImageCopy bltInfo {
            .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .srcSubresource.mipLevel = 0,
            .srcSubresource.baseArrayLayer = 0,
            .srcSubresource.layerCount = 1,
            .srcOffset.x = 0,
            .srcOffset.y = 0,
            .srcOffset.z = 0,
            .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .dstSubresource.mipLevel = 0,
            .dstSubresource.baseArrayLayer = 0,
            .dstSubresource.layerCount = 1,
            .dstOffset.x = 0,
            .dstOffset.y = 0,
            .dstOffset.z = 0,
            .extent.width = imgWidth,
            .extent.height = imgHeight,
            .extent.depth = 1
        };
        //在image之间进行复制操作
        vkCmdCopyImage(gfxCmd, stageImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       tex_obj->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                       &bltInfo);

        setImageLayout(gfxCmd, tex_obj->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }

    CALL_VK(vkEndCommandBuffer(gfxCmd))
    //命令同步
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(deviceInfo->device, &fenceInfo, nullptr, &fence));

    VkSubmitInfo submitInfo = {
            .pNext = nullptr,
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &gfxCmd,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };
    CALL_VK(vkQueueSubmit(deviceInfo->queue, 1, &submitInfo, fence) != VK_SUCCESS);
    CALL_VK(vkWaitForFences(deviceInfo->device, 1, &fence, VK_TRUE, 100000000) !=
            VK_SUCCESS);
    vkDestroyFence(deviceInfo->device, fence, nullptr);

    vkFreeCommandBuffers(deviceInfo->device, cmdPool, 1, &gfxCmd);
    vkDestroyCommandPool(deviceInfo->device, cmdPool, nullptr);
    if (stageImage != VK_NULL_HANDLE) {
        vkDestroyImage(deviceInfo->device, stageImage, nullptr);
        vkFreeMemory(deviceInfo->device, stageMem, nullptr);
    }

    return VK_SUCCESS;
}

void VKTextureManager::createTexture(VKDeviceManager *deviceInfo, uint8_t *m_pBuffer, size_t m_width,
                                size_t m_height) {
    for (int i = 0; i < kTextureCount; i++) {
        loadTexture(deviceInfo,m_pBuffer, texType[i], m_width, m_height, &textures[i], VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        const VkSamplerCreateInfo sampler {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0.0f,
                .maxAnisotropy = 1,
                .compareOp = VK_COMPARE_OP_NEVER,
                .minLod = 0.0f,
                .maxLod = 0.0f,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
                .unnormalizedCoordinates = VK_FALSE,
        };
        VkImageViewCreateInfo view {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .image = VK_NULL_HANDLE,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = kTextureFormat,
                .components = {
                        VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                        VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
                .flags = 0,
        };

        CALL_VK(vkCreateSampler(deviceInfo->device, &sampler, nullptr, &textures[i].sampler));
        view.image = textures[i].image;
        CALL_VK(vkCreateImageView(deviceInfo->device, &view, nullptr, &textures[i].view));
    }
}

void VKTextureManager::createImgTexture(VKDeviceManager *deviceInfo, AAssetManager *manager) {
    const char* textTexFiles = "sample_tex.png";


    LoadTextureFromFile(deviceInfo,manager,textTexFiles, &testTexture[0], VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    const VkSamplerCreateInfo sampler = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .maxAnisotropy = 1,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
    };

    VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .components =
                    {
                            VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                            VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
                    },
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            .flags = 0,
    };

    CALL_VK(vkCreateSampler(deviceInfo->device, &sampler, nullptr,
                            &testTexture[0].sampler));
    view.image = testTexture[0].image;
    CALL_VK(
            vkCreateImageView(deviceInfo->device, &view, nullptr, &testTexture[0].view));
}

VkResult VKTextureManager::loadTexture(VKDeviceManager *deviceInfo, uint8_t *buffer,
                                       VKTextureManager::TextureType type, size_t width,
                                       size_t height, VKTextureManager::VulkanTexture *texture,
                                       VkImageUsageFlags usage, VkFlags required_props) {

    if (!(usage | required_props)) {
        LOGE("No usage and required_pros");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    // Check for linear supportability
    VkFormatProperties props;
    bool needBlit = true;
    vkGetPhysicalDeviceFormatProperties(deviceInfo->physicalDevice, kTextureFormat, &props);
    assert((props.linearTilingFeatures | props.optimalTilingFeatures) & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        // linear format supporting the required texture
        needBlit = false;
    }

    size_t offset = getBufferOffset(texture, type, width, height);

    VkImageCreateInfo imageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = kTextureFormat,
            .extent = {static_cast<uint32_t>(texture->width), static_cast<uint32_t>(texture->height), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_LINEAR,
            // usage 指定图像如何使用
            .usage = (needBlit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                               : VK_IMAGE_USAGE_SAMPLED_BIT),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &deviceInfo->queueFamilyIndex,
            .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
            .flags = 0,
    };

    VkMemoryAllocateInfo memAlloc = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = 0,
            .memoryTypeIndex = 0,
    };

    VkMemoryRequirements memReqs;
    CALL_VK(vkCreateImage(deviceInfo->device, &imageCreateInfo, nullptr, &texture->image));
    vkGetImageMemoryRequirements(deviceInfo->device, texture->image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    VK_CHECK(allocateMemoryTypeFromProperties(deviceInfo,memReqs.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                              &memAlloc.memoryTypeIndex));

    CALL_VK(vkAllocateMemory(deviceInfo->device, &memAlloc, nullptr, &texture->mem));
    CALL_VK(vkBindImageMemory(deviceInfo->device, texture->image, texture->mem, 0));

    if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        const VkImageSubresource subres = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .arrayLayer = 0,
        };
        // 查询资源的布局信息
        vkGetImageSubresourceLayout(deviceInfo->device, texture->image, &subres, &texture->layout);
        CALL_VK(vkMapMemory(deviceInfo->device, texture->mem, 0, memAlloc.allocationSize, 0, &texture->mapped));

        copyTextureData(texture, buffer + offset);
    }


    texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = deviceInfo->queueFamilyIndex,
    };

    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(deviceInfo->device, &cmdPoolCreateInfo, nullptr, &cmdPool));

    VkCommandBuffer gfxCmd;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    CALL_VK(vkAllocateCommandBuffers(deviceInfo->device, &cmd, &gfxCmd));
    VkCommandBufferBeginInfo cmdBufferInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd, &cmdBufferInfo));

    // If linear is supported, we are done
    VkImage stageImage = VK_NULL_HANDLE;
    VkDeviceMemory stageMem = VK_NULL_HANDLE;
    if (!needBlit) {
        setImageLayout(gfxCmd, texture->image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    } else {
        // save current image and mem as staging image and memory
        stageImage = texture->image;
        stageMem = texture->mem;
        texture->image = VK_NULL_HANDLE;
        texture->mem = VK_NULL_HANDLE;

        // Create a tile texture to blit into
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage =
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        CALL_VK(vkCreateImage(deviceInfo->device, &imageCreateInfo, nullptr,
                              &texture->image));
        vkGetImageMemoryRequirements(deviceInfo->device, texture->image, &memReqs);

        memAlloc.allocationSize = memReqs.size;
        VK_CHECK(allocateMemoryTypeFromProperties(deviceInfo,
                                                  memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                  &memAlloc.memoryTypeIndex));
        CALL_VK(
                vkAllocateMemory(deviceInfo->device, &memAlloc, nullptr, &texture->mem));
        CALL_VK(vkBindImageMemory(deviceInfo->device, texture->image, texture->mem, 0));

        // transitions image out of UNDEFINED type
        setImageLayout(gfxCmd, stageImage, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        setImageLayout(gfxCmd, texture->image, VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        VkImageCopy bltInfo{
                .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .srcSubresource.mipLevel = 0,
                .srcSubresource.baseArrayLayer = 0,
                .srcSubresource.layerCount = 1,
                .srcOffset.x = 0,
                .srcOffset.y = 0,
                .srcOffset.z = 0,
                .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .dstSubresource.mipLevel = 0,
                .dstSubresource.baseArrayLayer = 0,
                .dstSubresource.layerCount = 1,
                .dstOffset.x = 0,
                .dstOffset.y = 0,
                .dstOffset.z = 0,
                .extent.width = static_cast<uint32_t>(texture->width),
                .extent.height = static_cast<uint32_t>(texture->height),
                .extent.depth = 1,
        };
        vkCmdCopyImage(gfxCmd, stageImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                       &bltInfo);

        setImageLayout(gfxCmd, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }

    CALL_VK(vkEndCommandBuffer(gfxCmd));
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(deviceInfo->device, &fenceInfo, nullptr, &fence));

    VkSubmitInfo submitInfo = {
            .pNext = nullptr,
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &gfxCmd,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };
    CALL_VK(vkQueueSubmit(deviceInfo->queue, 1, &submitInfo, fence) != VK_SUCCESS);
    CALL_VK(vkWaitForFences(deviceInfo->device, 1, &fence, VK_TRUE, 100000000) != VK_SUCCESS);
    vkDestroyFence(deviceInfo->device, fence, nullptr);

    vkFreeCommandBuffers(deviceInfo->device, cmdPool, 1, &gfxCmd);
    vkDestroyCommandPool(deviceInfo->device, cmdPool, nullptr);
    if (stageImage != VK_NULL_HANDLE) {
        vkDestroyImage(deviceInfo->device, stageImage, nullptr);
        vkFreeMemory(deviceInfo->device, stageMem, nullptr);
    }
    return VK_SUCCESS;
}

void VKTextureManager::setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                                      VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                                      VkPipelineStageFlags srcStages,
                                      VkPipelineStageFlags destStages) {
    //栏栅内存
   VkImageMemoryBarrier imageMemoryBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,
        .dstAccessMask = 0,
        .oldLayout = oldImageLayout,
        .newLayout = newImageLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    switch (oldImageLayout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }
    switch (newImageLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask =
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }
    //向命令管道栅栏，让命令按输入顺序执行
    vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
}

size_t VKTextureManager::getBufferOffset(VKTextureManager::VulkanTexture *texture,
                                         VKTextureManager::TextureType type, size_t width,
                                         size_t height) {
    size_t offset = 0;
    if (type == tTexY) {
        texture->width = width;
        texture->height = height;
    } else if (type == tTexU) {
        texture->width = width / 2;
        texture->height = height / 2;
        offset = width * height;
    } else if (type == tTexV) {
        texture->width = width / 2;
        texture->height = height / 2;
        offset = width * height * 5 / 4;
    }
    return offset;
}

void VKTextureManager::copyTextureData(VKTextureManager::VulkanTexture *texture, uint8_t *data) {
    uint8_t* mappedData = (uint8_t*)texture->mapped;
    for (int i = 0; i < texture->height; ++i) {
        memcpy(mappedData, data, texture->width);
        mappedData += texture->layout.rowPitch;
        data +=texture->width;
    }
}

VkResult
VKTextureManager::allocateMemoryTypeFromProperties(VKDeviceManager *deviceInfo, uint32_t typeBits,
                                                   VkFlags requirements_mask, uint32_t *typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((deviceInfo->memoryProperties.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return VK_SUCCESS;
            }
        }
        typeBits >>= 1;
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

void VKTextureManager::deleteTextures(VKDeviceManager *deviceInfo) {
    for (int i = 0; i < kTextureCount; ++i) {
        vkDestroyImageView(deviceInfo->device, textures[i].view, nullptr);
        vkDestroyImage(deviceInfo->device, textures[i].image, nullptr);
        vkDestroySampler(deviceInfo->device, textures[i].sampler, nullptr);
        vkUnmapMemory(deviceInfo->device, textures[i].mem);
        vkFreeMemory(deviceInfo->device, textures[i].mem, nullptr);
    }
}

void VKTextureManager::updateTextures(VKDeviceManager *deviceInfo, uint8_t *buffer, size_t width,
                                      size_t height) {
    for (int i = 0; i < kTextureCount; ++i) {
        size_t offset = getBufferOffset(&textures[i], texType[i], width, height);
        copyTextureData(&textures[i], buffer + offset);
    }
}
