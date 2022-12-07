//
// Created by glumes on 2021/3/7.
//

#ifndef VULKANCAMERA_FILTERUTIL_H
#define VULKANCAMERA_FILTERUTIL_H

//#include <vulkan/filter/VulkanFilter.h>
#include <string>

class VulkanFilter;

using namespace std;
class FilterUtil {
public:
    static VulkanFilter* getFilterByType(int type);

    static VulkanFilter* getFilterByType(string type);

    static float getProcess(int progress, float start, float end);
};


#endif //VULKANCAMERA_FILTERUTIL_H
