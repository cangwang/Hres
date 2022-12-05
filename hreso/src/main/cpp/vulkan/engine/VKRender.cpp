//
// Created by asus on 2022/11/8.
//

#include "VKRender.h"

VKRender::VKRender() {

}

VKRender::~VKRender() {

}

int VKRender::createRenderPass(VKDeviceManager *deviceInfo, VKSwapChainManager *swapChainInfo) {
    //子通道作用与后续的渲染操作，并依赖之前渲染通道输出到帧缓冲区的内容。比如说后处理效果的序列通常每一步都依赖之前的操作。如果将这些渲染操作分组到一个渲染通道中，通过Vulkan将通道中的渲染操作进行重排序，可以节省内存从而获得更好的性能
    // Create render pass
    VkAttachmentDescription attachmentDescriptions {
            .format = swapChainInfo->displayFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            //VK_ATTACHMENT_LOAD_OP_LOAD: 保存已经存在于当前附件的内容
            //VK_ATTACHMENT_LOAD_OP_CLEAR: 起始阶段以一个常量清理附件内容
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            //VK_ATTACHMENT_STORE_OP_STORE: 渲染的内容会存储在内存，并在之后进行读取操作
            //VK_ATTACHMENT_STORE_OP_DONT_CARE: 帧缓冲区的内容在渲染操作完毕后设置为undefined
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            //loadOp和storeOp应用在颜色和深度数据，同时stencilLoadOp / stencilStoreOp应用在模版数据
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            //VK_IMAGE_LAYOUT_COLOR_ATTACHMET_OPTIMAL: 图像作为颜色附件
            //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: 图像在交换链中被呈现
            //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: 图像作为目标，用于内存COPY操作
            //finalLayout指定当渲染通道结束自动变换时使用的布局
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    //每个子通道引用一个或者多个之前使用结构体描述的附件
    //当子通道开始的时候Vulkan会自动转变附件到这个layout。因为我们期望附件起到颜色缓冲区的作用，layout设置为VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL会给我们最好的性能
    VkAttachmentReference colourReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .flags = 0,
            .inputAttachmentCount = 0,
            //pInputAttachments: 附件从着色器中读取
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colourReference,
            //pResolveAttachments: 附件用于颜色附件的多重采样
            .pResolveAttachments = nullptr,
            //pDepthStencilAttachment: 附件用于深度和模版数据
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            //pPreserveAttachments: 附件不被子通道使用，但是数据被保存
            .pPreserveAttachments = nullptr,
    };
    VkRenderPassCreateInfo renderPassCreateInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescriptions,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 0,
            .pDependencies = nullptr,
    };
    CALL_VK(vkCreateRenderPass(deviceInfo->device, &renderPassCreateInfo, nullptr, &renderPass));

    return VK_SUCCESS;
}

int VKRender::createCommandPool(VKDeviceManager *deviceInfo, VKSwapChainManager *swapChainInfo,
                                VKBufferManager *bufferInfo, VKOffScreen *vkOffScreenInfo,
                                VulkanFilter *filter, VulkanFilter *offScreenFilter,
                                VulkanFilter *effectFilter) {
    //创建命令池
    VkCommandPoolCreateInfo cmdPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = 0,
    };
    CALL_VK(vkCreateCommandPool(deviceInfo->device, &cmdPoolCreateInfo, nullptr, &cmdPool))

    //创建命令内存
    // Record a command buffer that just clear the screen
    // 1 command buffer draw in 1 framebuffer
    // In our case we need 2 command as we have 2 framebuffer
    cmdBufferLen = swapChainInfo->swapchainLength;
    cmdBuffer = std::make_unique<VkCommandBuffer[]>(swapChainInfo->swapchainLength);
    VkCommandBufferAllocateInfo cmdBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = cmdBufferLen,
    };
    CALL_VK(vkAllocateCommandBuffers(deviceInfo->device, &cmdBufferCreateInfo, cmdBuffer.get()));

    for (int bufferIndex = 0; bufferIndex < swapChainInfo->swapchainLength; ++bufferIndex) {

        VkCommandBufferBeginInfo cmdBufferBeginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        CALL_VK(vkBeginCommandBuffer(cmdBuffer[bufferIndex], &cmdBufferBeginInfo));

        // transition the buffer into color attachment
        setImageLayout(cmdBuffer[bufferIndex],
                       swapChainInfo->displayImages[bufferIndex],
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        {

            FilterBuffer buffer{
                    .vertexBuffer = bufferInfo->vertexBuffer,
                    .indexBuffer = bufferInfo->indexBuffer,
                    .indexCount = static_cast<int>(bufferInfo->m_indexCount)
            };
            offScreenFilter->bindFilterBuffer(buffer);

            FilterFramebuffer framebuffer {
                    .framebuffer = vkOffScreenInfo->offscreenPass.frameBuffer[0],
                    .width = vkOffScreenInfo->offscreenPass.width,
                    .height = vkOffScreenInfo->offscreenPass.height
            };
            offScreenFilter->bindFilterFramebuffer(framebuffer);

            offScreenFilter->buildRenderPass(cmdBuffer[bufferIndex]);

            FilterBuffer effectBuffer{
                    .vertexBuffer = bufferInfo->vertexBuffer,
                    .indexBuffer = bufferInfo->indexBuffer,
                    .indexCount = static_cast<int>(bufferInfo->m_indexCount)
            };
            effectFilter->bindFilterBuffer(effectBuffer);

            FilterFramebuffer effectFrameBuffer{
                    .framebuffer = vkOffScreenInfo->offscreenPass.frameBuffer[1],
                    .width = vkOffScreenInfo->offscreenPass.width,
                    .height = vkOffScreenInfo->offscreenPass.height
            };
            effectFilter->bindFilterFramebuffer(effectFrameBuffer);
            effectFilter->buildRenderPass(cmdBuffer[bufferIndex]);
        }

        FilterBuffer buffer{
                .vertexBuffer = bufferInfo->vertexBuffer,
                .indexBuffer = bufferInfo->indexBuffer,
                .indexCount = (int)bufferInfo->m_indexCount,
        };
        filter->bindFilterBuffer(buffer);

        FilterFramebuffer framebuffer{
                .framebuffer = swapChainInfo->framebuffers[bufferIndex],
                .width = (int)swapChainInfo->imageSize.width,
                .height= (int)swapChainInfo->imageSize.height,
        };
        filter->bindFilterFramebuffer(framebuffer);

        filter->buildRenderPass(cmdBuffer[bufferIndex]);

        setImageLayout(cmdBuffer[bufferIndex],
                       swapChainInfo->displayImages[bufferIndex],
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        CALL_VK(vkEndCommandBuffer(cmdBuffer[bufferIndex]));
    }

    // We need to create a fence to be able, in the main loop, to wait for our
    // draw command(s) to finish before swapping the framebuffers
    VkFenceCreateInfo fenceCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreateFence(deviceInfo->device, &fenceCreateInfo, nullptr, &fence))

    // We need to create a semaphore to be able to wait, in the main loop, for our
    // framebuffer to be available for us before drawing.
    VkSemaphoreCreateInfo semaphoreCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreateSemaphore(deviceInfo->device, &semaphoreCreateInfo, nullptr, &semaphore))

    return VK_SUCCESS;
}

int VKRender::deleteCommandPool(VKDeviceManager *deviceInfo) {
    if (deviceInfo->device != nullptr) {
        vkFreeCommandBuffers(deviceInfo->device, cmdPool, cmdBufferLen, cmdBuffer.get());
        vkDestroyCommandPool(deviceInfo->device, cmdPool, nullptr);
        vkDestroyFence(deviceInfo->device, fence, nullptr);
        vkDestroySemaphore(deviceInfo->device, semaphore, nullptr);
    }
    return VK_SUCCESS;
}

//https://blog.csdn.net/qq_19473837/article/details/84913676
void VKRender::setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldImageLayout,
                         VkImageLayout newImageLayout, VkPipelineStageFlags srcStages,
                         VkPipelineStageFlags destStages) {
    //图像内存屏障 内存屏障用于定义命令缓冲区执行中的时刻，后续命令应等待先前的命令完成其操作
    VkImageMemoryBarrier imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = NULL,
            .srcAccessMask = 0,
            .dstAccessMask = 0,
            .oldLayout = oldImageLayout,
            .newLayout = newImageLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
                    {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1,
                    },
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
    //创建命令屏障
    vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                         &imageMemoryBarrier);
}
