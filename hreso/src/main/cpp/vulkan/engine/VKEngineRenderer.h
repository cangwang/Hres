//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKENGINERENDERER_H
#define HRES_VKENGINERENDERER_H


#include <vulkan/VideoRenderer.h>
#include <vulkan_wrapper/vulkan_wrapper.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <vulkan/engine/VKDeviceManager.h>
#include <vulkan/engine/VKSwapChainManager.h>
#include <vulkan/engine/VKRender.h>
#include <vulkan/engine/VKTextureManager.h>
#include <vulkan/engine/VKOffScreen.h>
#include <vulkan/filter/VulkanFilter.h>
#include <vulkan/filter/EffectFilter.h>
#include <vulkan/filter/FilterUtil.h>


class VKEngineRenderer : public VideoRenderer {
public:
    VKEngineRenderer();
    virtual ~VKEngineRenderer();

    virtual void init(ANativeWindow* window, size_t width, size_t height,AAssetManager* manager) override;
    virtual void initWindow(ANativeWindow* window, size_t width, size_t height) override;
    virtual void render() override;
    virtual void updateFrame(const video_frame& frame) override;
    virtual void draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation) override;
    virtual void setParameters(uint32_t params) override;
    virtual void setProcess(uint32_t process) override ;
    virtual uint32_t getParameters() override;
    virtual bool createTextures() override;
    virtual bool updateTextures() override;
    virtual void deleteTextures() override;

private:
    void createDevice(ANativeWindow* platformWindow, VkApplicationInfo* appInfo);
    void createSwapChain();
    void createRenderPass();
    void createFrameBuffers(VkImageView depthView = VK_NULL_HANDLE);

    void createUniformBuffers();
    void createVertexBuffer();
    void createIndexBuffer();

    void createCommandPool();

    void deleteUniformBuffers();
    void deleteCommandPool();

    VKDeviceManager* vkDeviceInfo;
    VKSwapChainManager* vkSwapChainInfo;
    VKRender* vkRenderInfo;
    VKBufferManager* vkBufferInfo;
    VKTextureManager* vkTextureInfo;
    VKOffScreen* vkOffScreenInfo;

    uint8_t *m_pBuffer;
    size_t m_length;

    VulkanFilter *vulkanFilter;
    OffScreenFilter *offscreenFilter;
    VulkanFilter* effectFilter;

    size_t m_filter = 0;
    uint32_t m_CurrentProcess = 0;
    uint32_t m_LastProcess = 0;

    void createOffscreenReaderPassAndFramebuffer();
};


#endif //HRES_VKENGINERENDERER_H
