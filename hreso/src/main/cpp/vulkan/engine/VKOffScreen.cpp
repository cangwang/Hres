//
// Created by asus on 2022/11/8.
//

#include "VKOffScreen.h"

void VKOffScreen::createOffscreen(VKDeviceManager *deviceInfo, VKSwapChainManager *swapchain) {
    offscreenPass.width = swapchain->displaySize.width;
    offscreenPass.height = swapchain->displaySize.height;

    offscreenPass.frameBuffer.resize(num);
    offscreenPass.sampler.resize(num);
    offscreenPass.color.resize(num);
    offscreenPass.descriptor.resize(num);

    for (int i = 0; i < num; ++i) {
        VkImageCreateInfo image {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = swapchain->displayFormat,
            .extent = {
                .width = static_cast<uint32_t>(offscreenPass.width),
                .height =  static_cast<uint32_t>(offscreenPass.height),
            },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            //TODO 此处可能有问题
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
        };
        VkMemoryAllocateInfo mem_alloc {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = 0,
            .memoryTypeIndex = 0
        };

        VkMemoryRequirements mem_reqs;
        CALL_VK(vkCreateImage(deviceInfo->device, &image, nullptr,&offscreenPass.color[i].image))

        vkGetImageMemoryRequirements(deviceInfo->device, offscreenPass.color[i].image, &mem_reqs);

        mem_alloc.allocationSize = mem_reqs.size;
        VK_CHECK(allocateMemoryTypeFromProperties(deviceInfo,mem_reqs.memoryTypeBits,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                  &mem_alloc.memoryTypeIndex))

        CALL_VK(vkAllocateMemory(deviceInfo->device, &mem_alloc, nullptr, &offscreenPass.color[i].mem));
        CALL_VK(vkBindImageMemory(deviceInfo->device, offscreenPass.color[i].image, offscreenPass.color[i].mem, 0))

        VkImageViewCreateInfo colorImageView = {};
        colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorImageView.pNext = nullptr;
        colorImageView.image = offscreenPass.color[i].image;
        colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorImageView.format  = swapchain->displayFormat;
        colorImageView.subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
        };
        colorImageView.components = {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
        };

        CALL_VK(vkCreateImageView(deviceInfo->device, &colorImageView, nullptr,
                                  &offscreenPass.color[i].view))
        //采样结构体
        VkSamplerCreateInfo  samplerCreateInfo {
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
            .unnormalizedCoordinates = VK_FALSE
        };

        //创建采样体
        CALL_VK(vkCreateSampler(deviceInfo->device, & samplerCreateInfo, nullptr, &offscreenPass.sampler[i]))

        //包括image格式，采样次数和渲染前后的操作
        VkAttachmentDescription attachmentDescription {
            .format = swapchain->displayFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, //颜色内容在绘制前需要清空
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,  //绘制后需要保留
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, //深度内容在绘制前清空
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, //深度内容在绘制前清空
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            //可能要修改的地方
            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkAttachmentReference colorReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        //Subpass负责对attachments的读写操作
        VkSubpassDescription subpassDescription {
                //Todo
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
//                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_BEGIN_RANGE,
                .flags = 0,
                .inputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorReference,
                .pResolveAttachments = nullptr,
                // 不使用深度模板
                .pDepthStencilAttachment = nullptr,
                .preserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr,
        };
        //Render pass封装了attachment description和subpass。一般render pass包含两个attachment description（延迟渲染会更多）和至少一个subpass（复杂效果会更多）。
        VkRenderPassCreateInfo  renderPassCreateInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescription,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 0,
            .pDependencies = nullptr,
        };

        CALL_VK(vkCreateRenderPass(deviceInfo->device, &renderPassCreateInfo, nullptr, &offscreenPass.renderPass))

        VkImageView attachements[1];
        attachements[0] = offscreenPass.color[i].view;
        // renderpass 到framebuffer
        VkFramebufferCreateInfo fbCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = offscreenPass.renderPass,
            .layers = 1,
            .attachmentCount = 1,
            .pAttachments = attachements,
            .width = static_cast<uint32_t>(offscreenPass.width),
            .height = static_cast<uint32_t>(offscreenPass.height),
        };

        vkCreateFramebuffer(deviceInfo->device, &fbCreateInfo, nullptr, &offscreenPass.frameBuffer[i]);
        offscreenPass.descriptor[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        offscreenPass.descriptor[i].imageView = offscreenPass.color[i].view;
        offscreenPass.descriptor[i].sampler = offscreenPass.sampler[i];
    }
}

int
VKOffScreen::updateOffScreenDescriptorSet(VKDeviceManager *deviceInfo, VKBufferManager *vkbufferInfo,
                                          VKTextureManager *textureInfo) {
    //Buffer 描述符
    VkDescriptorBufferInfo bufferInfo {
            .buffer = vkbufferInfo->uboBuffer,
            .offset = 0,
            .range = sizeof (UniformBufferObject)
    };

    VkDescriptorImageInfo texDsts[3];
    memset(texDsts, 0, sizeof(texDsts));
    for (int32_t idx = 0; idx < 3; ++idx) {
        texDsts[idx].sampler = textureInfo->textures[idx].sampler;
        texDsts[idx].imageView = textureInfo->textures[idx].view;
        texDsts[idx].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }

    //以WriteDescriptorSet的形式来完成一个Buffer和Descriptor的binding工作
    VkWriteDescriptorSet writeDst[2] {
            {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = offscreenPipeline.descSet_,
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &bufferInfo,
                    .pTexelBufferView = nullptr
            },
            {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext = nullptr,
                    .dstSet = offscreenPipeline.descSet_,
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 3,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = texDsts,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr
            }
    };

    vkUpdateDescriptorSets(deviceInfo->device, 2, writeDst, 0, nullptr);

    return VK_SUCCESS;
}

VkResult
VKOffScreen::allocateMemoryTypeFromProperties(VKDeviceManager *deviceInfo, uint32_t typeBits,
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
    // No memory types matched, return failure
    return VK_ERROR_MEMORY_MAP_FAILED;
}
