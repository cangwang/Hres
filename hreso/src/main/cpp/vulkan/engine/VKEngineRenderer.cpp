//
// Created by asus on 2022/11/8.
//

#include "VKEngineRenderer.h"

VKEngineRenderer::VKEngineRenderer(): offscreenFilter(nullptr), effectFilter(nullptr), vulkanFilter(nullptr) {
    vkRenderInfo = new VKRender;
    vkBufferInfo = new VKBufferManager;
    vkSwapChainInfo = new VKSwapChainManager;
    vkDeviceInfo = new VKDeviceManager;
    vkTextureInfo = new VKTextureManager;
    vkOffScreenInfo = new VKOffScreen;

    vkDeviceInfo->initialized = false;

    m_pBuffer = nullptr;
    m_length = 0;
    path.clear();
}

VKEngineRenderer::~VKEngineRenderer() {
    vkDeviceInfo->initialized = false;
    if (useYUV) {
        deleteTextures();
    } else {
        deleteImageTextures();
    }
    deleteUniformBuffers();
    deleteCommandPool();
    path.clear();
    options = nullptr;
}

void
VKEngineRenderer::init(ANativeWindow *window, size_t width, size_t height, AAssetManager *manager) {
    m_backingWidth = width;
    m_backingHeight = height;

    aAssetManager = manager;

    if (!InitVulkan()) {
        LOGE("Vulkan is unavailable, install vulkan and re-start");
        return;
    }
    vulkanFilter = new VulkanFilter;
//    offscreenFilter = new OffScreenFilter;
    offscreenFilter = new VulkanFilter;
    this->filterType = options->getFilterType();
    effectFilter = FilterUtil::getFilterByType(options->getFilterType());

    VkApplicationInfo appInfo {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .apiVersion = VK_MAKE_VERSION(1, 0, 0),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .pApplicationName = "camera2GLPreview",
            .pEngineName = "camera",
    };
    createDevice(window, &appInfo);
    createSwapChain();
}

void VKEngineRenderer::initWindow(ANativeWindow *window, IOptions* options) {
    if (options == nullptr) {
        HLOGE("initWindow options is null");
        return;
    }

    this->options = options;
    m_backingWidth = options->srcWidth;
    m_backingHeight = options->srcHeight;
    if (!InitVulkan()) {
        LOGE("Vulkan is unavailable, install vulkan and re-start");
        return;
    }

    vulkanFilter = new VulkanFilter;
//    offscreenFilter = new OffScreenFilter;
    offscreenFilter = new VulkanFilter;
    this->filterType = options->getFilterType();
    effectFilter = FilterUtil::getFilterByType(options->getFilterType());
    if (options != nullptr) {
        effectFilter->setOption(options);
    }
    VkApplicationInfo appInfo {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .apiVersion = VK_MAKE_VERSION(1, 0, 0),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .pApplicationName = "dillImage",
            .pEngineName = "camera",
    };
    createDevice(window, &appInfo);
    createSwapChain(options->srcWidth, options->srcHeight);
//    createSwapChain();
}

void VKEngineRenderer::initOffscreen(IOptions* options) {
    if (options == nullptr) {
        HLOGE("initWindow options is null");
        return;
    }

    this->options = options;
    m_backingWidth = options->srcWidth;
    m_backingHeight = options->srcHeight;
    if (!InitVulkan()) {
        LOGE("Vulkan is unavailable, install vulkan and re-start");
        return;
    }

//    this->filterType = options->getFilterType();
//    offscreenFilter = FilterUtil::getFilterByType(options->getFilterType());
//    offscreenFilter->setOption(options);

    offscreenFilter = new VulkanFilter;
    this->filterType = options->getFilterType();
    effectFilter = FilterUtil::getFilterByType(options->getFilterType());
    effectFilter->setOption(options);

    VkApplicationInfo appInfo {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .apiVersion = VK_MAKE_VERSION(1, 0, 0),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .pApplicationName = "offscreenImage",
            .pEngineName = "camera",
    };
    createOffscreenDevice(&appInfo);
    createOffscreenChain(options->srcWidth, options->srcHeight);
}

void VKEngineRenderer::render() {
    uint32_t nextIndex;
    //等待信号获取交换链上的图像
    VkResult res = vkAcquireNextImageKHR(vkDeviceInfo->device, vkSwapChainInfo->swapchain, UINT64_MAX, vkRenderInfo->semaphore, VK_NULL_HANDLE, &nextIndex);
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
//        recreateSwapChain();
        HLOGV("VK_ERROR_OUT_OF_DATE_KHR");
        return;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    CALL_VK(vkResetFences(vkDeviceInfo->device, 1, &vkRenderInfo->fence))

    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        //设置等待信号量
        .pWaitSemaphores = &vkRenderInfo->semaphore,
        .pWaitDstStageMask = &waitStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkRenderInfo->cmdBuffer[nextIndex],
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr
    };

    if (vkDeviceInfo->queue == nullptr){
        LOGE("zhy vkDeviceInfo->queue == nullptr");
        return ;
    }

    //使用vkQueueSubmit函数向图像队列提交命令缓冲区。当开销负载比较大的时候，处于效率考虑，函数可以持有VkSubmitInfo结构体数组。最后一个参数引用了一个可选的栅栏，当命令缓冲区执行完毕时候它会被发送信号
    CALL_VK(vkQueueSubmit(vkDeviceInfo->queue,1,&submitInfo,vkRenderInfo->fence));
    CALL_VK(vkWaitForFences(vkDeviceInfo->device,1,&vkRenderInfo->fence,VK_TRUE,100000000));

    //将结果提交到交换链，使其最终显示在屏幕上。Presentation通过VkPresentInfoKHR结构体配置，具体位置在drawFrame函数最后
    VkResult result;
    VkPresentInfoKHR presentInfo {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &vkSwapChainInfo->swapchain,
            .pImageIndices = &nextIndex,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pResults = &result
    };
    //vkQueuePresentKHR函数提交请求呈现交换链中的图像
    vkQueuePresentKHR(vkDeviceInfo->queue,&presentInfo);
}

void VKEngineRenderer::renderOffscreen() {
    CALL_VK(vkResetFences(vkDeviceInfo->device, 1, &vkRenderInfo->fence))

    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            //设置等待信号量
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = &waitStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &vkRenderInfo->cmdBuffer[vkRenderInfo->cmdBufferLen - 1],
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
    };

    if (vkDeviceInfo->queue == nullptr){
        LOGE("zhy vkDeviceInfo->queue == nullptr");
        return ;
    }

    //使用vkQueueSubmit函数向图像队列提交命令缓冲区。当开销负载比较大的时候，处于效率考虑，函数可以持有VkSubmitInfo结构体数组。最后一个参数引用了一个可选的栅栏，当命令缓冲区执行完毕时候它会被发送信号
    CALL_VK(vkQueueSubmit(vkDeviceInfo->queue,1,&submitInfo,vkRenderInfo->fence));
    CALL_VK(vkWaitForFences(vkDeviceInfo->device,1,&vkRenderInfo->fence,VK_TRUE,100000000));
    vkQueueWaitIdle(vkDeviceInfo->queue);
}

void VKEngineRenderer::updateFrame(const video_frame &frame) {

}
//YUV输入使用
void VKEngineRenderer::draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation) {
    useYUV = true;
    m_pBuffer = buffer;
    m_length = length;
    m_rotation = rotation;

    m_width = width;
    m_height = height;

    if (!vkDeviceInfo->initialized) {
        createRenderPass();
        createFrameBuffers();

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createTextures();

        createOffscreenReaderPassAndFramebuffer();

        offscreenFilter->init(vkDeviceInfo->device, vkOffScreenInfo->offscreenPass.renderPass);

        vector<VkDescriptorBufferInfo> vecBufferInfo;
        vecBufferInfo.resize(1);

        VkDescriptorBufferInfo bufferInfo {
                .buffer = vkBufferInfo->uboBuffer,
                .offset = 0,
                .range = sizeof (UniformBufferObject)
        };

        vecBufferInfo[0] = bufferInfo;

        LOGI("zhy vec buffer info size is %d",vecBufferInfo.size());

        vector<VkDescriptorImageInfo> vecImageInfo;
        vecImageInfo.resize(3);
        VkDescriptorImageInfo texDsts[3];
        memset(texDsts, 0, sizeof(texDsts));
        for (int i = 0; i < 3; ++i) {
            texDsts[i].sampler = vkTextureInfo->textures[i].sampler;
            texDsts[i].imageView = vkTextureInfo->textures[i].view;
            texDsts[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            vecImageInfo[i] = texDsts[i];
        }

        LOGI("zhy vec image info size is %d", vecImageInfo.size());

        offscreenFilter->updateDescriptorSet(vecBufferInfo,vecImageInfo);

        effectFilter->init(vkDeviceInfo->device,vkOffScreenInfo->offscreenPass.renderPass);

        effectFilter->updateDescriptorSet(vkOffScreenInfo->offscreenPass.descriptor[0].sampler,
                                          vkOffScreenInfo->offscreenPass.descriptor[0].imageView,
                                          VK_IMAGE_LAYOUT_GENERAL);

        vulkanFilter->init(vkDeviceInfo->device,vkRenderInfo->renderPass);

        vulkanFilter->updateDescriptorSet(
                vkOffScreenInfo->offscreenPass.descriptor[1].sampler,
                vkOffScreenInfo->offscreenPass.descriptor[1].imageView,
                VK_IMAGE_LAYOUT_GENERAL
        );

        createCommandPool();
        vkDeviceInfo->initialized = true;
    }

    if (vkDeviceInfo->initialized && m_params < 1){

    } else if (m_params != m_filter) { //切换滤镜
        m_filter = m_params;

        LOGI("zhy ew filter");
        delete effectFilter;
        effectFilter = nullptr;

        effectFilter = FilterUtil::getFilterByType(m_filter);

        effectFilter->init(vkDeviceInfo->device,vkOffScreenInfo->offscreenPass.renderPass);

        std::vector<VkDescriptorBufferInfo> vecBufferInfo;
        vecBufferInfo.resize(1);

        VkDescriptorBufferInfo bufferInfo {
                bufferInfo.buffer = vkBufferInfo->rgbBuffer,
                bufferInfo.offset = 0,
                bufferInfo.range = sizeof(RGBBufferObject),
        };

        vecBufferInfo[0] = bufferInfo;


        std::vector<VkDescriptorImageInfo> imageInfoList;
        VkDescriptorImageInfo imageInfo{
                .sampler = vkOffScreenInfo->offscreenPass.descriptor[0].sampler,
                .imageView = vkOffScreenInfo->offscreenPass.descriptor[0].imageView,
                .imageLayout = vkOffScreenInfo->offscreenPass.descriptor[0].imageLayout
        };

        imageInfoList.push_back(imageInfo);

        effectFilter->updateDescriptorSet(vecBufferInfo,imageInfoList);

//        effectFilter->updateImageDescriptorSet(imageInfoList,0);

        createCommandPool();
    }

    if (m_CurrentProcess != m_LastProcess) {
        m_LastProcess = m_CurrentProcess;
        createCommandPool();
        LOGE("zhy m_CurrentProcess != m_LastProcess create command pool");
    }

    updateTextures();

    if (vkDeviceInfo->initialized) {
        render();
    }
}

void VKEngineRenderer::drawImg() {
    if (options == nullptr) {
        HLOGE("drawImg options is null");
        return;
    }
    this->path = options->getAddress();
    m_length = options->srcWidth * options->srcHeight * 4;
    m_rotation = options->getRotation();

    m_width = options->srcWidth;
    m_height = options->srcHeight;

    if (!vkDeviceInfo->initialized) {
        createRenderPass();
        createFrameBuffers();

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createImageTextures();

        createOffscreenReaderPassAndFramebuffer();

        offscreenFilter->init(vkDeviceInfo->device, vkOffScreenInfo->offscreenPass.renderPass);

        vector<VkDescriptorBufferInfo> vecBufferInfo;
        vecBufferInfo.resize(1);

        VkDescriptorBufferInfo bufferInfo {
                .buffer = vkBufferInfo->uboBuffer,
                .offset = 0,
                .range = sizeof (UniformBufferObject)
        };

        vecBufferInfo[0] = bufferInfo;

        LOGI("zhy vec buffer info size is %d",vecBufferInfo.size());

        vector<VkDescriptorImageInfo> vecImageInfo;
        vecImageInfo.resize(1);
        VkDescriptorImageInfo texDsts[1];
        memset(texDsts, 0, sizeof(texDsts));

        texDsts[0].sampler = vkTextureInfo->testTexture[0].sampler;
        texDsts[0].imageView = vkTextureInfo->testTexture[0].view;
        texDsts[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        vecImageInfo[0] = texDsts[0];


        LOGI("zhy vec image info size is %d", vecImageInfo.size());

        offscreenFilter->updateDescriptorSet(vecBufferInfo,vecImageInfo);
        if (options != nullptr) {
            effectFilter->setOption(options);
        }
        effectFilter->init(vkDeviceInfo->device,vkOffScreenInfo->offscreenPass.renderPass);

        effectFilter->updateDescriptorSet(vkOffScreenInfo->offscreenPass.descriptor[0].sampler,
                                          vkOffScreenInfo->offscreenPass.descriptor[0].imageView,
                                          VK_IMAGE_LAYOUT_GENERAL);

        createImageCommandPool();
        vkDeviceInfo->initialized = true;
    }

    if (vkDeviceInfo->initialized && m_params < 1){

    } else if (this->filterType != filterType){ //切换滤镜
        LOGI("zhy ew filter");
        delete effectFilter;
        effectFilter = nullptr;

        this->filterType = filterType;
        effectFilter = FilterUtil::getFilterByType(filterType);
        if (options != nullptr) {
            effectFilter->setOption(options);
        }
        effectFilter->init(vkDeviceInfo->device,vkOffScreenInfo->offscreenPass.renderPass);

        std::vector<VkDescriptorBufferInfo> vecBufferInfo;
        vecBufferInfo.resize(1);

        VkDescriptorBufferInfo bufferInfo {
                bufferInfo.buffer = vkBufferInfo->rgbBuffer,
                bufferInfo.offset = 0,
                bufferInfo.range = sizeof(RGBBufferObject),
        };

        vecBufferInfo[0] = bufferInfo;

        std::vector<VkDescriptorImageInfo> imageInfoList;
        VkDescriptorImageInfo imageInfo{
                .sampler = vkOffScreenInfo->offscreenPass.descriptor[0].sampler,
                .imageView = vkOffScreenInfo->offscreenPass.descriptor[0].imageView,
                .imageLayout = vkOffScreenInfo->offscreenPass.descriptor[0].imageLayout
        };

        imageInfoList.push_back(imageInfo);

        effectFilter->updateDescriptorSet(vecBufferInfo,imageInfoList);

//        effectFilter->updateImageDescriptorSet(imageInfoList,0);

        createImageCommandPool();
    }

    if (m_CurrentProcess != m_LastProcess) {
        m_LastProcess = m_CurrentProcess;
        createImageCommandPool();
        LOGE("zhy m_CurrentProcess != m_LastProcess create command pool");
    }


    if (vkDeviceInfo->initialized) {
        render();
    }
    vkTextureInfo->saveImage(vkDeviceInfo, options->getSaveAddress().c_str(), vkSwapChainInfo->lastDisplayImage);
}

void VKEngineRenderer::drawOffscreenImg() {
    HLOGV("drawOffscreenImg");
    if (options == nullptr) {
        HLOGE("drawImg options is null");
        return;
    }
    this->path = options->getAddress();
    m_length = options->srcWidth * options->srcHeight * 4;
    m_rotation = options->getRotation();

    m_width = options->srcWidth;
    m_height = options->srcHeight;

    if (!vkDeviceInfo->initialized) {
        createOffscreenRenderPass();
        createOffscreenFrameBuffers();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createImageTextures();
        //创建离线渲染的image
        createOffscreenReaderPassAndFramebuffer(offscreenFormat, m_width, m_height);

        offscreenFilter->init(vkDeviceInfo->device, vkOffScreenInfo->offscreenPass.renderPass);

        vector<VkDescriptorBufferInfo> vecBufferInfo;
        vecBufferInfo.resize(1);

        VkDescriptorBufferInfo bufferInfo {
                .buffer = vkBufferInfo->uboBuffer,
                .offset = 0,
                .range = sizeof (UniformBufferObject)
        };

        vecBufferInfo[0] = bufferInfo;

        LOGI("zhy vec buffer info size is %d",vecBufferInfo.size());

        vector<VkDescriptorImageInfo> vecImageInfo;
        vecImageInfo.resize(1);
        VkDescriptorImageInfo texDsts[1];
        memset(texDsts, 0, sizeof(texDsts));

        texDsts[0].sampler = vkTextureInfo->testTexture[0].sampler;
        texDsts[0].imageView = vkTextureInfo->testTexture[0].view;
        texDsts[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        vecImageInfo[0] = texDsts[0];

        LOGI("zhy vec image info size is %d", vecImageInfo.size());

        offscreenFilter->updateDescriptorSet(vecBufferInfo,vecImageInfo);
        if (options != nullptr) {
            effectFilter->setOption(options);
        }
        effectFilter->init(vkDeviceInfo->device, vkOffScreenInfo->offscreenPass.renderPass);

        effectFilter->updateDescriptorSet(vkOffScreenInfo->offscreenPass.descriptor[0].sampler,
                                          vkOffScreenInfo->offscreenPass.descriptor[0].imageView,
                                          VK_IMAGE_LAYOUT_GENERAL);

//        createOffscreenImageCommandPool();
        createImageCommandPool();
        vkDeviceInfo->initialized = true;
    }

    if (m_CurrentProcess != m_LastProcess) {
        m_LastProcess = m_CurrentProcess;
//        createOffscreenImageCommandPool();
        createImageCommandPool();
        LOGE("zhy m_CurrentProcess != m_LastProcess create command pool");
    }

    if (vkDeviceInfo->initialized) {
        renderOffscreen();
    }
    vkTextureInfo->saveImage(vkDeviceInfo, options->getSaveAddress().c_str(), vkSwapChainInfo->lastDisplayImage);
}

void VKEngineRenderer::setParameters(uint32_t params) {
    m_params = params;
}

void VKEngineRenderer::setProcess(uint32_t process) {
    m_CurrentProcess = process;

    if (effectFilter != nullptr){
        effectFilter->setProcess(m_CurrentProcess);
    }
}

void VKEngineRenderer::setOption(IOptions *option) {
    this->options = option;
    if (effectFilter != nullptr){
        effectFilter->setOption(option);
    }
}

uint32_t VKEngineRenderer::getParameters() {
    return 0;
}

bool VKEngineRenderer::createTextures() {
    vkTextureInfo->createTexture(vkDeviceInfo,m_pBuffer,m_width,m_height);
    return true;
}

bool VKEngineRenderer::createImageTextures() {
    vkTextureInfo->createImageTexture(vkDeviceInfo, path, m_width, m_height);
    return true;
}
//YUV使用
bool VKEngineRenderer::updateTextures() {
    vkTextureInfo->updateTextures(vkDeviceInfo,m_pBuffer,m_width,m_height);
    return true;
}

void VKEngineRenderer::deleteTextures() {
    vkTextureInfo->deleteTextures(vkDeviceInfo);
}

void VKEngineRenderer::deleteImageTextures() {
    vkTextureInfo->deleteImageTextures(vkDeviceInfo);
}

void VKEngineRenderer::createDevice(ANativeWindow *platformWindow, VkApplicationInfo *appInfo) {
    vkDeviceInfo->createDevice(platformWindow, appInfo);
}

void VKEngineRenderer::createOffscreenDevice(VkApplicationInfo *appInfo) {
    vkDeviceInfo->createOffscreenDevice(appInfo);
}

void VKEngineRenderer::createSwapChain() {
    vkSwapChainInfo->createSwapChain(vkDeviceInfo);
}

void VKEngineRenderer::createSwapChain(int width, int height) {
    vkSwapChainInfo->createSwapChain(vkDeviceInfo, width, height);
}

void VKEngineRenderer::createOffscreenChain(int width, int height) {
    vkSwapChainInfo->createOffscreen(offscreenFormat, width, height);
}

void VKEngineRenderer::createRenderPass() {
    vkRenderInfo->createRenderPass(vkDeviceInfo, vkSwapChainInfo);
}

void VKEngineRenderer::createOffscreenRenderPass() {
    vkRenderInfo->createRenderPass(vkDeviceInfo, offscreenFormat);
}

void VKEngineRenderer::createFrameBuffers(VkImageView depthView) {
    vkSwapChainInfo->createFrameBuffer(vkDeviceInfo, &vkRenderInfo->renderPass, depthView);
}

void VKEngineRenderer::createOffscreenFrameBuffers(VkImageView depthView) {
    vkSwapChainInfo->createOffscreenFrameBuffer(vkDeviceInfo, &vkRenderInfo->renderPass, depthView);
}

void VKEngineRenderer::createUniformBuffers() {
    vkBufferInfo->updateUniformBuffers(m_width,m_height,m_rotation,m_backingWidth,m_backingHeight);
    //创建ubo数据
    vkBufferInfo->createUniformBuffers(vkDeviceInfo);
    vkBufferInfo->createRGBUniformBuffer(vkDeviceInfo);
}

void VKEngineRenderer::createVertexBuffer() {
    vkBufferInfo->createVertexBuffer(vkDeviceInfo);
    vkBufferInfo->createShowVertexBuffer(vkDeviceInfo);
}

void VKEngineRenderer::createIndexBuffer() {
    vkBufferInfo->createIndexBuffer(vkDeviceInfo);
}

void VKEngineRenderer::createCommandPool() {
    vkRenderInfo->createCommandPool(vkDeviceInfo, vkSwapChainInfo, vkBufferInfo, vkOffScreenInfo, vulkanFilter, offscreenFilter,
                                    effectFilter);
}

void VKEngineRenderer::createImageCommandPool() {
    vkRenderInfo->createCommandPool(vkDeviceInfo, vkSwapChainInfo, vkBufferInfo, vkOffScreenInfo, offscreenFilter,
                                    effectFilter);
}

void VKEngineRenderer::createOffscreenImageCommandPool() {
    vkRenderInfo->createOffscreenCommandPool(vkDeviceInfo, vkBufferInfo, vkOffScreenInfo, offscreenFilter,
                                    effectFilter);
}

void VKEngineRenderer::deleteUniformBuffers() {
    vkDestroyBuffer(vkDeviceInfo->device, vkBufferInfo->uboBuffer, nullptr);
    vkFreeMemory(vkDeviceInfo->device, vkBufferInfo->uboBufferMemory, nullptr);
}

void VKEngineRenderer::deleteCommandPool() {
    vkRenderInfo->deleteCommandPool(vkDeviceInfo);
}

void VKEngineRenderer::createOffscreenReaderPassAndFramebuffer() {
    vkOffScreenInfo->createOffscreen(vkDeviceInfo,vkSwapChainInfo);
}

VkImage VKEngineRenderer::createOffscreenReaderPassAndFramebuffer(VkFormat format, int width, int height) {
    return vkOffScreenInfo->createOffscreen(vkDeviceInfo, format, width, height);
}
