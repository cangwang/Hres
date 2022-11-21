//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKBUFFERMANAGER_H
#define HRES_VKBUFFERMANAGER_H

#include <vulkan_wrapper/vulkan_wrapper.h>
#include <vector>
#include <string>
#include <vulkan/Log.h>
#include <vulkan/engine/VKDefine.h>
#include <vulkan/engine/VKDeviceManager.h>
#include <vulkan/CommonUtils.h>

class VKBufferManager {
public:
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkBuffer uboBuffer;
    VkDeviceMemory uboBufferMemory;

    VkBuffer rgbBuffer;
    VkDeviceMemory rgbBufferMemory;

    uint32_t m_indexCount;
    VkBuffer showVertexBuffer;
    VkDeviceMemory showBufferMemory;

    int createRGBUniformBuffer(VKDeviceManager* deviceInfo);

    int createVertexBuffer(VKDeviceManager* deviceInfo);

    int createShowVertexBuffer(VKDeviceManager* deviceInfo);

    int createIndexBuffer(VKDeviceManager* deviceInfo);

    int createUniformBuffers(VKDeviceManager* deviceInfo);

    int updateUniformBuffers(size_t m_width,size_t m_height,int m_rotation,size_t m_backingWidth,size_t m_backingHeight);

    UniformBufferObject m_ubo;

    RGBBufferObject m_rgb;
private:
    bool mapMemoryTypeToIndex(VKDeviceManager* deviceInfo, uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex);

    void createBuffer(VKDeviceManager * deviceInfo, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBuffer(VKDeviceManager * deviceInfo, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};


#endif //HRES_VKBUFFERMANAGER_H
