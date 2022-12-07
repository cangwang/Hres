//
// Created by asus on 2022/11/13.
//
#include "FilterUtil.h"

#include <vulkan/filter/VulkanFilter.h>
#include <vulkan/filter/FsrVulkanFilter.h>

VulkanFilter* FilterUtil::getFilterByType(int type) {
//    switch (type){
//        case SHOW_TYPE:
//            return new VulkanFilter();
//        case RBG_FILTER_TYPE:
//            return new RGBFilter();
//        case MIRROR_FILTER_TYPE:
//            return new MirrorFilter();
//        case REVERT_FILTER_TYPE:
//            return new ColorInvertFilter();
//        case EXPOSURE_FILTER_TYPE:
//            return new ExposureFilter();
//        case GAMMA_FILTER_TYPE:
//            return new GammFilter();
//        case CONTRAST_FILTER_TYPE:
//            return new ContrastFilter();
//        case HAZE_FILTER_TYPE:
//            return new HazeFilter();
//        default:
//            break;
//    }
    return new VulkanFilter;
}

VulkanFilter *FilterUtil::getFilterByType(string type) {
    if (type == "fsr") {
        return new FsrVulkanFilter;
    } else {
        return new VulkanFilter;
    }
}

float FilterUtil::getProcess(int progress, float start, float end) {
    return (end - start) * progress / 100.0f + start;
}
