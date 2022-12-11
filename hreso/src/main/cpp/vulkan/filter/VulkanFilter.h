//
// Created by asus on 2022/11/9.
//

#ifndef HRES_VULKANFILTER_H
#define HRES_VULKANFILTER_H

#include <vulkan_wrapper/vulkan_wrapper.h>
#include <vulkan/filter/FilterDefine.h>
#include <vulkan/VKUtils.h>
#include <vulkan/VKShader.h>
#include <vulkan/filter/VulkanInitializers.h>
#include <vulkan/filter/FilterUtil.h>
#include <vulkan/Log.h>
#include <array>
#include <bean/ioptions.h>

using namespace std;
class VulkanFilter {
public:
    VulkanFilter() : pVertexShader(showVertexShader), pFragShader(showFragShader) {
        pushConstant.resize(0);
    }
    ~VulkanFilter();

    virtual int init(VkDevice device,VkRenderPass renderPass);

    virtual int buildRenderPass(VkCommandBuffer commandBuffer,VkRenderPass renderPass,VkFramebuffer framebuffer);

    virtual int buildRenderPass(VkCommandBuffer commandBuffer);

    virtual int bindFilterBuffer(FilterBuffer& buffer);
    virtual int bindFilterFramebuffer(FilterFramebuffer& framebuffer);

    virtual int updateDescriptorSet(VkSampler sampler,VkImageView imageView,VkImageLayout imageLayout);

    virtual int updateDescriptorSet(std::vector<VkDescriptorBufferInfo>& bufferInfo,std::vector<VkDescriptorImageInfo>& imageInfo);

    virtual int updateBufferDescriptorSet(std::vector<VkDescriptorBufferInfo>& bufferInfo,int binding = 0);

    virtual int updateImageDescriptorSet(std::vector<VkDescriptorImageInfo>& imageInfo,int binding = 0 );

    virtual void setProcess(uint32_t process);

    virtual void setOption(IOptions * option);
protected:
    const char* pVertexShader = showVertexShader;
    const char* pFragShader = showFragShader;

    virtual int createDescriptorLayout();
    virtual int createDescriptorSet();
    virtual int createPipelineCache();
    virtual int createPipeline(VkRenderPass renderPass);
    virtual int createPipeline();

    struct FilterPipeline pipeline;
    struct FilterContext context;

    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;

    vector<float> pushConstant;

    int indexCount;

    int width;
    int height;

    bool isInit = false;

    uint32_t mProcess ;

    IOptions* options;
};


#endif //HRES_VULKANFILTER_H
