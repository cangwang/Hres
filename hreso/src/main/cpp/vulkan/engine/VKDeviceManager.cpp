//
// Created by asus on 2022/11/8.
//

#include "VKDeviceManager.h"

VKDeviceManager::VKDeviceManager() {

}

VKDeviceManager::~VKDeviceManager() {

}

int VKDeviceManager::createDevice(ANativeWindow *platformWindow, VkApplicationInfo *appInfo) {
    vector<const char*> instance_extensions;
    vector<const char*> device_extensions;

    instance_extensions.push_back("VK_KHR_surface");
    instance_extensions.push_back("VK_KHR_android_surface");

    device_extensions.push_back("VK_KHR_swapchain");

    // 创建instance
    VkInstanceCreateInfo instanceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        // application 结构体
        .pApplicationInfo = appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
        .ppEnabledExtensionNames = instance_extensions.data(),
        // layer结构体
        .enabledLayerCount = 0,
        // layer结构体
        .ppEnabledLayerNames = nullptr
    };
    // 创建instance
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance))
    //创建android surface信息
    VkAndroidSurfaceCreateInfoKHR createInfo {
        .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .window = platformWindow
    };
    //创建surface
    CALL_VK(vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface))

    //获取gpu数量
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr))
    //获取GPU设备
    VkPhysicalDevice tmpGpus[gpuCount];
    CALL_VK(vkEnumeratePhysicalDevices(instance, &gpuCount, tmpGpus))
    physicalDevice = tmpGpus[0];

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    // Find a GFX queue family
    //寻找队列族
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount);
    vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    for (int i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            this->queueFamilyIndex = i;
            break;
        }
    }

    //创建一个vulkan设备
    float priorities[] = {
            1.0f
    };
    //设备队列信息
    VkDeviceQueueCreateInfo queueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCount = 1,
        .queueFamilyIndex = this->queueFamilyIndex,
        .pQueuePriorities = priorities
    };

    VkDeviceCreateInfo deviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
        .ppEnabledExtensionNames = device_extensions.data(),
        .pEnabledFeatures = nullptr
    };
    //创建设备
    CALL_VK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device))
    vkGetDeviceQueue(device, 0, 0, &queue);

    return VK_SUCCESS;
}

int VKDeviceManager::createOffscreenDevice(VkApplicationInfo *appInfo) {
    vector<const char*> instance_extensions;
    vector<const char*> device_extensions;

    instance_extensions.push_back("VK_KHR_surface");
    instance_extensions.push_back("VK_KHR_android_surface");
    device_extensions.push_back("VK_KHR_swapchain");

    // 创建instance
    VkInstanceCreateInfo instanceCreateInfo {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            // application 结构体
            .pApplicationInfo = appInfo,
            .enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
            .ppEnabledExtensionNames = instance_extensions.data(),
            // layer结构体
            .enabledLayerCount = 0,
            // layer结构体
            .ppEnabledLayerNames = nullptr
    };
    // 创建instance
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance))

    //获取gpu数量
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr))
    //获取GPU设备
    VkPhysicalDevice tmpGpus[gpuCount];
    CALL_VK(vkEnumeratePhysicalDevices(instance, &gpuCount, tmpGpus))
    physicalDevice = tmpGpus[0];

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    // Find a GFX queue family
    //寻找队列族
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount);
    vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    for (int i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            this->queueFamilyIndex = i;
            break;
        }
    }

    //创建一个vulkan设备
    float priorities[] = {
            1.0f
    };
    //设备队列信息
    VkDeviceQueueCreateInfo queueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCount = 1,
            .queueFamilyIndex = this->queueFamilyIndex,
            .pQueuePriorities = priorities
    };

    VkDeviceCreateInfo deviceCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
            .ppEnabledExtensionNames = device_extensions.data(),
            .pEnabledFeatures = nullptr
    };
    //创建设备
    CALL_VK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device))
    vkGetDeviceQueue(device, 0, 0, &queue);

    return VK_SUCCESS;
}


