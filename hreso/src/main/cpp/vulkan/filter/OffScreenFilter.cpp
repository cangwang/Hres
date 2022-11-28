//
// Created by asus on 2022/11/12.
//

#include "OffScreenFilter.h"

int OffScreenFilter::init(VkDevice device, VkRenderPass renderPass) {
    if (isInit){
        return VK_SUCCESS;
    }
    context.device = device;
    context.renderPass = renderPass;
    createDescriptorLayout();
    createPipelineCache();
    createPipeline();
    createDescriptorSet();
    isInit = true;
    return VK_SUCCESS;
}

int OffScreenFilter::updateDescriptorSet(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout) {
    VkDescriptorImageInfo texDsts[1];
    memset(texDsts, 0, sizeof(texDsts));
    texDsts->sampler = sampler;
    texDsts->imageView = imageView;
    texDsts->imageLayout = imageLayout;
    VkWriteDescriptorSet writeDst = vks::writeDescriptorSet(pipeline.descSet_,
                                                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                            0,
                                                            texDsts);

    vkUpdateDescriptorSets(context.device, 1, &writeDst, 0, nullptr);
    return VK_SUCCESS;
}

int OffScreenFilter::updateDescriptorSet(vector<VkDescriptorBufferInfo> &bufferInfo,
                                         vector<VkDescriptorImageInfo> &imageInfo) {
    std::vector<VkWriteDescriptorSet> writeDst;

    writeDst.push_back(vks::writeDescriptorSet(pipeline.descSet_,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,0,bufferInfo.data(),bufferInfo.size()));

    writeDst.push_back(vks::writeDescriptorSet(pipeline.descSet_,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,imageInfo.data(),imageInfo.size()));

    vkUpdateDescriptorSets(context.device,writeDst.size(),writeDst.data(),0, nullptr);

    return VK_SUCCESS;
}

//https://blog.csdn.net/qq_19473837/article/details/85399124
int OffScreenFilter::createDescriptorLayout() {
    //descriptor set layout 是用来描述某个 descriptor set 列表的内容
    std::vector<VkDescriptorSetLayoutBinding> binding;

    binding.push_back(vks::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,0,1));

    binding.push_back(vks::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1,3));

    VkDescriptorSetLayoutCreateInfo info = vks::descriptorSetLayoutCreateInfo(binding.data(), binding.size());

    CALL_VK(vkCreateDescriptorSetLayout(context.device,&info, nullptr,&pipeline.descLayout_));

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = vks::pipelineLayoutCreateInfo(&pipeline.descLayout_,1);

    CALL_VK(vkCreatePipelineLayout(context.device,&pipelineLayoutCreateInfo, nullptr,&pipeline.layout_));

    return VK_SUCCESS;
}

int OffScreenFilter::createDescriptorSet() {
    std::vector<VkDescriptorPoolSize> poolSize ;
    poolSize.push_back(vks::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1));
    poolSize.push_back(vks::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,3));


    VkDescriptorPoolCreateInfo poolCreateInfo = vks::descriptorPoolCreateInfo(
            poolSize.size(), poolSize.data(), 2);

    CALL_VK(vkCreateDescriptorPool(context.device,&poolCreateInfo, nullptr,&pipeline.descPool_));

    VkDescriptorSetAllocateInfo allocateInfo = vks::descriptorSetAllocateInfo(pipeline.descPool_,&pipeline.descLayout_,1);

    CALL_VK(vkAllocateDescriptorSets(context.device,&allocateInfo,&pipeline.descSet_));
    return VK_SUCCESS;
}
