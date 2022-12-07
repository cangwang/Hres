//
// Created by asus on 2022/12/7.
//

#include "FsrVulkanFilter.h"

void FsrVulkanFilter::setOption(IOptions *option) {
    pushConstant[0] = option->srcWidth;
    pushConstant[1] = option->srcHeight;
}