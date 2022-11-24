//
// Created by asus on 2022/11/9.
//

#ifndef HRES_FILTERDEFINE_H
#define HRES_FILTERDEFINE_H

#include <vulkan_wrapper/vulkan_wrapper.h>

typedef struct  FilterPipeline{
    VkDescriptorSetLayout descLayout_;
    VkDescriptorPool descPool_;
    VkDescriptorSet descSet_;
    VkPipelineLayout layout_;
    VkPipeline pipeline_;
    VkPipelineCache cache_;
} FilterPipeline;

typedef struct FilterContext{
    VkRenderPass renderPass;
    VkDevice device;
    VkFramebuffer framebuffer;
} FilterContext;


typedef struct FilterBuffer{
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    int indexCount;
} FilterBuffer;

typedef struct FilterFramebuffer{
    VkFramebuffer framebuffer;
    int width;
    int height;
} FilterFramebuffer;

#endif //HRES_FILTERDEFINE_H
