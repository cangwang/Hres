//
// Created by asus on 2022/11/8.
//

#include "VKBufferManager.h"

int VKBufferManager::createRGBUniformBuffer(VKDeviceManager *deviceInfo) {
    m_rgb.red = 1.0;
    m_rgb.green = 0.0;
    m_rgb.blue = 1.0;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize bufferSize = sizeof(m_rgb);

    createBuffer(deviceInfo,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    CALL_VK(vkMapMemory(deviceInfo->device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, &m_rgb, bufferSize);
    vkUnmapMemory(deviceInfo->device, stagingBufferMemory);

    createBuffer(deviceInfo,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 rgbBuffer, rgbBufferMemory);

    copyBuffer(deviceInfo,stagingBuffer, rgbBuffer, bufferSize);

    vkDestroyBuffer(deviceInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(deviceInfo->device, stagingBufferMemory, nullptr);
    return 0;
}

//https://blog.csdn.net/yuxing55555/article/details/89011064
int VKBufferManager::createVertexBuffer(VKDeviceManager *deviceInfo) {
    const Vertex vertices[4] {
            { {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } },
            { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }
    };

    //图片的buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory  stagingBufferMemory;
    VkDeviceSize bufferSize = sizeof(vertices);
    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT 创建映射到CPU端的内存
    // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT cpu端的内存对显卡课件，且访问是不需要特殊操作
    createBuffer(deviceInfo, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);
    void* data = nullptr;
    //将cpu的数据memcpy到GPU显存当中
    CALL_VK(vkMapMemory(deviceInfo->device, stagingBufferMemory, 0, bufferSize, 0, &data))
    memcpy(data, vertices, bufferSize);
    vkUnmapMemory(deviceInfo->device, stagingBufferMemory);
    //创建映射到顶点数据的内存,并映射的vertexBuffer和vertexBufferMemory当中
    createBuffer(deviceInfo, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
    //传输cpu图片buffer，顶点数据
    copyBuffer(deviceInfo, stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(deviceInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(deviceInfo->device, stagingBufferMemory, nullptr);

    return VK_SUCCESS;
}

int VKBufferManager::createShowVertexBuffer(VKDeviceManager *deviceInfo) {
    const float vertexData[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    };

    VkBufferCreateInfo bufferInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = sizeof(vertexData),
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &deviceInfo->queueFamilyIndex,
    };

    CALL_VK(vkCreateBuffer(deviceInfo->device, &bufferInfo, nullptr,
                           &showVertexBuffer))

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(deviceInfo->device, showVertexBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReq.size,
            .memoryTypeIndex = 0,  // Memory type assigned in the next step
    };
    // Assign the proper memory type for that buffer
    mapMemoryTypeToIndex(deviceInfo,memReq.memoryTypeBits,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);

    CALL_VK(vkAllocateMemory(deviceInfo->device, &allocInfo, nullptr, &showBufferMemory))


    void* data;
    CALL_VK(vkMapMemory(deviceInfo->device, showBufferMemory, 0, allocInfo.allocationSize,
                        0, &data))
    memcpy(data, vertexData, sizeof(vertexData));
    vkUnmapMemory(deviceInfo->device, showBufferMemory);

    CALL_VK(vkBindBufferMemory(deviceInfo->device, showVertexBuffer, showBufferMemory, 0))

    return VK_SUCCESS;
}

int VKBufferManager::createIndexBuffer(VKDeviceManager *deviceInfo) {
    const uint16_t indices[6] {
            0, 1, 2, 2, 3, 0
    };

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    m_indexCount = sizeof(indices) / sizeof(indices[0]);
    VkDeviceSize bufferSize = sizeof(indices);

    createBuffer(deviceInfo,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    CALL_VK(vkMapMemory(deviceInfo->device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, indices, bufferSize);
    vkUnmapMemory(deviceInfo->device, stagingBufferMemory);

    createBuffer(deviceInfo,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 indexBuffer, indexBufferMemory);

    copyBuffer(deviceInfo,stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(deviceInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(deviceInfo->device, stagingBufferMemory, nullptr);
    return 0;
}

int VKBufferManager::createUniformBuffers(VKDeviceManager *deviceInfo) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize bufferSize = sizeof(m_ubo);

    createBuffer(deviceInfo,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    CALL_VK(vkMapMemory(deviceInfo->device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, &m_ubo, bufferSize);
    vkUnmapMemory(deviceInfo->device, stagingBufferMemory);

    createBuffer(deviceInfo,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 uboBuffer, uboBufferMemory);

    copyBuffer(deviceInfo,stagingBuffer, uboBuffer, bufferSize);

    vkDestroyBuffer(deviceInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(deviceInfo->device, stagingBufferMemory, nullptr);
    return 0;
}

int VKBufferManager::updateUniformBuffers(size_t m_width, size_t m_height, int m_rotation,
                                          size_t m_backingWidth, size_t m_backingHeight) {
    float targetAspectRatio = (float)m_width / (float) m_height;
    //比例缩放
    CommonUtils::mat4f_load_ortho(-1.0f, 1.0f, -targetAspectRatio, targetAspectRatio, -1.0f, 1.0f, m_ubo.projection);
    //角度
    CommonUtils::mat4f_load_rotation_z(m_rotation + 180, m_ubo.rotation);
    // 缩放大小
    float scaleFactor = CommonUtils::aspect_ratio_correction(false, m_backingWidth, m_backingHeight, m_width, m_height);
    CommonUtils::mat4f_load_scale(scaleFactor, scaleFactor, 1.0f, m_ubo.scale);
    return VK_SUCCESS;
}

bool VKBufferManager::mapMemoryTypeToIndex(VKDeviceManager *deviceInfo, uint32_t typeBits,
                                           VkFlags requirements_mask, uint32_t *typeIndex) {
    // 物理内存参数
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(deviceInfo->physicalDevice, &memoryProperties);
    //找具有适当参数并与图像内存要求兼容的内存类型
    for (uint32_t i = 0; i < 32; ++i) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }

    return false;
}
//创建缓冲--方便地使用不同的缓冲大小，内存类型来创建我们需要的缓冲
void VKBufferManager::createBuffer(VKDeviceManager *deviceInfo, VkDeviceSize size,
                                   VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                   VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    //buffer结构体
    VkBufferCreateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = size,
        .usage = usage,
        //使用 VK_SHARING_MODE_EXCLUSIVE 创建的图像对象 的图像子资源和缓冲区的区间必须 只能被同一个队列族中的队列在任何时间访问。
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &deviceInfo->queueFamilyIndex
    };

    //创建图像buffer
    CALL_VK(vkCreateBuffer(deviceInfo->device, &bufferInfo, nullptr, &buffer))

    //请求buffer内存
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(deviceInfo->device, buffer, &memReq);

    // https://blog.csdn.net/qq_19473837/article/details/84908424
    VkMemoryAllocateInfo allocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memReq.size,
        .memoryTypeIndex = 0
    };

    // 标记buffer内存类型
    mapMemoryTypeToIndex(deviceInfo, memReq.memoryTypeBits, properties, &allocateInfo.memoryTypeIndex);
    // 创建内存空间
    CALL_VK(vkAllocateMemory(deviceInfo->device, &allocateInfo, nullptr, &bufferMemory))
    // 绑定图像数据到内存
    CALL_VK(vkBindBufferMemory(deviceInfo->device, buffer, bufferMemory, 0))
}
//用于在缓冲之间复制数据
void VKBufferManager::copyBuffer(VKDeviceManager *deviceInfo, VkBuffer srcBuffer, VkBuffer dstBuffer,
                            VkDeviceSize size) {
    //命令池结构体
    VkCommandPoolCreateInfo cmdPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = deviceInfo->queueFamilyIndex,
    };

    //创建命令池
    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(deviceInfo->device, &cmdPoolCreateInfo, nullptr, &cmdPool));

    //命令buffer
    VkCommandBuffer cmdBuffer;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };
    //申请命令内存
    CALL_VK(vkAllocateCommandBuffers(deviceInfo->device, &cmd, &cmdBuffer))

    //开始提交命令
    VkCommandBufferBeginInfo cmdBufferInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(cmdBuffer, &cmdBufferInfo));

    //复制buffer
    VkBufferCopy copyRegion {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size,
    };
    vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    //结束命令
    CALL_VK(vkEndCommandBuffer(cmdBuffer))
    //创建栏栅
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(deviceInfo->device, &fenceInfo, nullptr, &fence));

    //创建提交信息的结构体
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr
    };
    //提交命令信息
    CALL_VK(vkQueueSubmit(deviceInfo->queue, 1, &submitInfo, fence))
    //等待函数运行结束栅栏状态变更
    CALL_VK(vkWaitForFences(deviceInfo->device, 1, &fence, VK_TRUE, 10000000))
    //释放栅栏
    vkDestroyFence(deviceInfo->device, fence, nullptr);
    //释放命令
    vkFreeCommandBuffers(deviceInfo->device, cmdPool, 1, &cmdBuffer);
    //释放命令池
    vkDestroyCommandPool(deviceInfo->device, cmdPool, nullptr);
}
