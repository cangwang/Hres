//
// Created by asus on 2022/12/7.
//

#include "FsrVulkanFilter.h"

void FsrVulkanFilter::setOption(IOptions *option) {
    w = option->srcWidth;
    h = option->srcHeight;
    pushConstant[0] = (float) option->srcWidth;
    pushConstant[1] = (float) option->srcHeight;
}