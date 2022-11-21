//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKTEXTUREMANAGER_H
#define HRES_VKTEXTUREMANAGER_H
#include <vulkan_wrapper/vulkan_wrapper.h>
#include <cassert>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <vulkan/engine/VKDeviceManager.h>

#include <util/stb_image.h>

#define LOG_TAG "VKTextureManager"
#define HLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define HLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

class VKTextureManager {
public:
    VKTextureManager(): texType{tTexY, tTexU, tTexV} {

    }
    static const VkFormat kTexFmt = VK_FORMAT_R8G8B8A8_UNORM;

    struct VulkanTexture {
        VkSampler sampler;
        VkImage image;
        VkImageLayout imageLayout;
        VkSubresourceLayout layout;
        VkDeviceMemory mem;
        VkImageView view;
        size_t width;
        size_t height;
        void* mapped;
    };

    typedef struct texture_object {
        VkSampler sampler;
        VkImage image;
        VkImageLayout imageLayout;
        VkDeviceMemory mem;
        VkImageView view;
        size_t tex_width;
        size_t tex_height;
    } texture_object;

    struct texture_object testTexture[1];

    static const VkFormat kTextureFormat = VK_FORMAT_R8_UNORM;
    static const uint32_t kTextureCount = 3;
    struct VulkanTexture textures[kTextureCount];
    enum TextureType {
        tTexY, tTexU, tTexV
    };

    const TextureType texType[kTextureCount];

    VkResult LoadTextureFromFile(VKDeviceManager * deviceInfo, AAssetManager* manager, const char* filePath,
                                 struct texture_object* tex_obj,
                                 VkImageUsageFlags usage, VkFlags required_props);


    void createTexture(VKDeviceManager *deviceInfo, uint8_t *m_pBuffer, size_t m_width, size_t m_height);

    void createImgTexture(VKDeviceManager *deviceInfo, AAssetManager* manager);

    VkResult loadTexture(VKDeviceManager *deviceInfo, uint8_t *buffer, TextureType type, size_t width,
                         size_t height,
                         VulkanTexture *texture, VkImageUsageFlags usage, VkFlags required_props);

    void setImageLayout(VkCommandBuffer cmdBuffer,
                        VkImage image,
                        VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout,
                        VkPipelineStageFlags srcStages,
                        VkPipelineStageFlags destStages);

    size_t getBufferOffset(VulkanTexture *texture, TextureType type, size_t width, size_t height);

    void copyTextureData(VulkanTexture *texture, uint8_t *data);


    VkResult allocateMemoryTypeFromProperties(VKDeviceManager *deviceInfo, uint32_t typeBits,
                                              VkFlags requirements_mask,
                                              uint32_t *typeIndex);

    void deleteTextures(VKDeviceManager *deviceInfo);

    void updateTextures(VKDeviceManager *deviceInfo, uint8_t *buffer, size_t width, size_t height);
};


#endif //HRES_VKTEXTUREMANAGER_H
