//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKDEVICEMANAGER_H
#define HRES_VKDEVICEMANAGER_H

#include <vulkan_wrapper/vulkan_wrapper.h>
#include <vector>
#include <string>
#include <vulkan/Log.h>
#include <android/native_window.h>

using namespace std;
class VKDeviceManager {
public:
    VKDeviceManager();
    ~VKDeviceManager();

    int createDevice(ANativeWindow* platformWindow, VkApplicationInfo* appInfo);
    int createOffscreenDevice(VkApplicationInfo *appInfo);

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkDevice device;

    uint32_t queueFamilyIndex;

    VkSurfaceKHR surface;
    VkQueue queue;

    bool initialized = false;
};


#endif //HRES_VKDEVICEMANAGER_H
