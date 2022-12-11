//
// Created by asus on 2022/11/9.
//

#include "VulkanFilter.h"

VulkanFilter::~VulkanFilter() {
    options = nullptr;
    pVertexShader = nullptr;
    pFragShader = nullptr;
    //清除缓存,其实缓存复用只能让创建压力减少，这里都是单次渲染并没有很大的利用价值
    vkDestroyPipelineCache(context.device, pipeline.cache_, nullptr);
}

int VulkanFilter::init(VkDevice device, VkRenderPass renderPass) {
    if (isInit){
        return VK_SUCCESS;
    }
    context.device = device;
    context.renderPass = renderPass;
    //管线需要使用描述符，需要先创建描述符
    createDescriptorLayout();
    createPipelineCache();
    createPipeline();
    createDescriptorSet();
    isInit = true;
    return VK_SUCCESS;
}

// https://zhuanlan.zhihu.com/p/49112352
int VulkanFilter::buildRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass,
                                  VkFramebuffer framebuffer) {
    //清屏颜色
    VkClearValue clearVals{
            .color.float32[0] = 0.0f,
            .color.float32[1] = 0.0f,
            .color.float32[2] = 0.0f,
            .color.float32[3] = 1.0f,
    };
    VkRenderPassBeginInfo offRenderInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea = {
            .extent.width = static_cast<uint32_t>(width),
            .extent.height = static_cast<uint32_t>(height),
        },
        .clearValueCount = 1,
        .pClearValues = &clearVals
    };
    //清屏命令
    vkCmdBeginRenderPass(commandBuffer, &offRenderInfo, VK_SUBPASS_CONTENTS_INLINE);

    if (pushConstant.size() != 0) {
        //将常量数据传给着色器
        vkCmdPushConstants(commandBuffer, pipeline.layout_, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstant.size()), pushConstant.data());
    }
    //视口
    //Viewport用于描述framebuffer作为渲染输出结果目标区域
    VkViewport viewport = vks::viewport((float)width, (float)height, 0.0f, 1.0f);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    //裁剪窗口大小
    VkRect2D scissor = vks::rect2D(width, height, 0, 0);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    //设置为commandBuffer当前活动管道
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline_);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.layout_, 0, 1, &pipeline.descSet_, 0 , nullptr);

    VkDeviceSize offset = 0;
    // 顶点buffer缓冲区绑定bindings
    // vkCmdBindVertexBuffers函数用于绑定顶点缓冲区，就像之前的设置一样，除了命令缓冲区之外，前两个参数指定了我们要为其指定的顶点缓冲区的偏移量和数量。最后两个参数指定了将要绑定的顶点缓冲区的数组及开始读取数据的起始偏移量。
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
    //绑定顶点索引
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    //设置绘制时的顶点索引
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

//    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    return VK_SUCCESS;
}

int VulkanFilter::buildRenderPass(VkCommandBuffer commandBuffer) {
    return buildRenderPass(commandBuffer,context.renderPass,context.framebuffer);
}

int VulkanFilter::bindFilterBuffer(FilterBuffer &buffer) {
    indexBuffer = buffer.indexBuffer;
    vertexBuffer = buffer.vertexBuffer;
    indexCount = buffer.indexCount;
    return 0;
}

int VulkanFilter::bindFilterFramebuffer(FilterFramebuffer &framebuffer) {
    context.framebuffer = framebuffer.framebuffer;
    width = framebuffer.width;
    height = framebuffer.height;
    return 0;
}

int VulkanFilter::updateDescriptorSet(VkSampler sampler, VkImageView imageView,
                                      VkImageLayout imageLayout) {
    VkDescriptorImageInfo texDsts[1];
    memset(texDsts, 0, sizeof(texDsts));
    texDsts->sampler = sampler;
    texDsts->imageView = imageView;
    texDsts->imageLayout = imageLayout;
    VkWriteDescriptorSet writeDst = vks::writeDescriptorSet(pipeline.descSet_,
                                                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                            0,
                                                            texDsts);
    //更新描述符集
    vkUpdateDescriptorSets(context.device, 1, &writeDst, 0, nullptr);
    return VK_SUCCESS;
}

int VulkanFilter::updateDescriptorSet(std::vector <VkDescriptorBufferInfo> &bufferInfo,
                                      std::vector <VkDescriptorImageInfo> &imageInfo) {
    std::vector<VkWriteDescriptorSet> writeDst ;
    //更新ubo数组
    writeDst.push_back(vks::writeDescriptorSet(pipeline.descSet_,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,0,bufferInfo.data(),bufferInfo.size()));

    writeDst.push_back(vks::writeDescriptorSet(pipeline.descSet_,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,imageInfo.data(),imageInfo.size()));

    vkUpdateDescriptorSets(context.device,writeDst.size(),writeDst.data(),0, nullptr);

    return VK_SUCCESS;
}

int VulkanFilter::updateBufferDescriptorSet(std::vector <VkDescriptorBufferInfo> &bufferInfo,
                                            int binding) {
    std::vector<VkWriteDescriptorSet> writeDst ;
    writeDst.push_back(vks::writeDescriptorSet(pipeline.descSet_,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,binding,bufferInfo.data(),bufferInfo.size()));
    vkUpdateDescriptorSets(context.device,writeDst.size(),writeDst.data(),0, nullptr);
    return VK_SUCCESS;
}

int VulkanFilter::updateImageDescriptorSet(std::vector <VkDescriptorImageInfo> &imageInfo,
                                           int binding) {
    std::vector<VkWriteDescriptorSet> writeDst ;
    writeDst.push_back(vks::writeDescriptorSet(pipeline.descSet_,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,binding,imageInfo.data(),imageInfo.size()));
    vkUpdateDescriptorSets(context.device,writeDst.size(),writeDst.data(),0, nullptr);
    return VK_SUCCESS;
}

void VulkanFilter::setProcess(uint32_t process) {
    mProcess = process;
}

void VulkanFilter::setOption(IOptions *option) {
    this->options = option;
}

int VulkanFilter::createDescriptorLayout() {
    //采样器描述符
    VkDescriptorSetLayoutBinding binding = vks::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

    VkDescriptorSetLayoutCreateInfo info = vks::descriptorSetLayoutCreateInfo(&binding, 1);

    CALL_VK(vkCreateDescriptorSetLayout(context.device,&info, nullptr,&pipeline.descLayout_));

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = vks::pipelineLayoutCreateInfo(&pipeline.descLayout_,1);

    if (pushConstant.size() != 0){
        VkPushConstantRange push_constant_ranges[1] = {};
        push_constant_ranges->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_ranges->offset = 0;
        push_constant_ranges->size = pushConstant.size() * sizeof(float);

        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = push_constant_ranges;
    }

    CALL_VK(vkCreatePipelineLayout(context.device,&pipelineLayoutCreateInfo, nullptr,&pipeline.layout_));

    return VK_SUCCESS;
}

//描述符集合不能直接创建，它们必须像命令缓冲区一样，从对象池中分配使用
int VulkanFilter::createDescriptorSet() {
    //创建描述符池
    VkDescriptorPoolSize size = vks::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                        1);
    VkDescriptorPoolCreateInfo poolCreateInfo = vks::descriptorPoolCreateInfo(1, &size, 1);

    CALL_VK(vkCreateDescriptorPool(context.device,&poolCreateInfo, nullptr,&pipeline.descPool_));
    //图像和采样器资源绑定到描述符集合中的具体描述符
    VkDescriptorSetAllocateInfo allocateInfo = vks::descriptorSetAllocateInfo(pipeline.descPool_,&pipeline.descLayout_,1);
    //分配vulkan描述符集，不需要显式清理描述符集合，因为它们会在描述符池销毁的时候自动释放
    CALL_VK(vkAllocateDescriptorSets(context.device,&allocateInfo,&pipeline.descSet_));
    return VK_SUCCESS;
}

int VulkanFilter::createPipelineCache() {
    VkPipelineCacheCreateInfo cacheCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext = nullptr,
        .initialDataSize = 0,
        .pInitialData = nullptr,
        .flags = 0
    };
    CALL_VK(vkCreatePipelineCache(context.device, &cacheCreateInfo, nullptr, &pipeline.cache_));
    return VK_SUCCESS;
}

int VulkanFilter::createPipeline(VkRenderPass renderPass) {
    //输入装配
    //https://blog.csdn.net/qq_35312463/article/details/103955952
    //VK_PRIMITIVE_TOPOLOGY_POINT_LIST: 顶点到点
    //VK_PRIMITIVE_TOPOLOGY_LINE_LIST: 两点成线，顶点不共用
    //VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: 两点成线，每个线段的结束顶点作为下一个线段的开始顶点
    //VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: 三点成面，顶点不共用
    //VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: 每个但教训的第二个、第三个顶点都作为下一个三角形的前两个顶点
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = vks::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,0,VK_FALSE);
    //纹理坐标映射
    //顶点输入绑定描述
    VkVertexInputBindingDescription inputBindingDescription = vks::vertexInputBindingDescription(0,5 *
                                                                                                   sizeof(float),VK_VERTEX_INPUT_RATE_VERTEX);
    //顶点输入属性描述
    VkVertexInputAttributeDescription inputAttributeDescription[2] = {
        //position顶点位置
        vks::vertexInputAttributeDescription(0,0,VK_FORMAT_R32G32B32_SFLOAT,0),
        //uv 纹理位置
        vks::vertexInputAttributeDescription(0,1,VK_FORMAT_R32G32_SFLOAT,sizeof(float) * 3)
    };
    // 顶点装配状态
    VkPipelineVertexInputStateCreateInfo inputStateCreateInfo = vks::pipelineVertexInputStateCreateInfo();
    inputStateCreateInfo.vertexBindingDescriptionCount = 1;
    inputStateCreateInfo.pVertexBindingDescriptions = &inputBindingDescription;
    inputStateCreateInfo.vertexAttributeDescriptionCount = 2;
    inputStateCreateInfo.pVertexAttributeDescriptions = inputAttributeDescription;

    // 绘制的方向不对也会导致渲染黑屏的
    //指定管线光栅化状态
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = vks::pipelineRasterizationStateCreateInfo(
            VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);

    // 全局混色配置
    VkPipelineColorBlendAttachmentState blendAttachmentState = vks::pipelineColorBlendAttachmentState(
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo blendStateCreateInfo = vks::pipelineColorBlendStateCreateInfo(
            1, &blendAttachmentState);

    // 深度测试不一定用得上，先写着，后面传 nullptr
    //深度测试状态
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = vks::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
    //指定管线饰扣和裁剪测试状态
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = vks::pipelineViewportStateCreateInfo(1,1,0);
    //配置多重采样
    //默认情况下，每个像素只使用一个采样点，这样就不会有重采样，在这种情况下，最终图像将保持不变
    VkPipelineMultisampleStateCreateInfo multisampleState = vks::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);

    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    //管线动态状态描述符
    VkPipelineDynamicStateCreateInfo dynamicState = vks::pipelineDynamicStateCreateInfo(dynamicStateEnables);
    //shader模型
    VkShaderModule vertexShader, fragmentShader;
    CALL_VK(buildShader(pVertexShader, VK_SHADER_STAGE_VERTEX_BIT, context.device, &vertexShader))
    CALL_VK(buildShader(pFragShader, VK_SHADER_STAGE_FRAGMENT_BIT, context.device, &fragmentShader))

    //shader创建状态
    VkPipelineShaderStageCreateInfo shaderStages[2]{
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertexShader,
                    .pSpecializationInfo = nullptr,
                    .flags = 0,
                    .pName = "main",
            },
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = fragmentShader,
                    .pSpecializationInfo = nullptr,
                    .flags = 0,
                    .pName = "main",
            }};
    //创建图形管线
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = vks::pipelineCreateInfo(pipeline.layout_, renderPass,0);
    //输入图元装配
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    //光栅化
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    //全局混色配置
    pipelineCreateInfo.pColorBlendState = &blendStateCreateInfo;
    //多重采样
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    //视口配置
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    // 不用深度测试，写 nullptr
    //深度测试
    pipelineCreateInfo.pDepthStencilState = nullptr;
    //动态状态
    pipelineCreateInfo.pDynamicState = &dynamicState;
    // shader个数
    pipelineCreateInfo.stageCount = 2;
    // shader阶段
    pipelineCreateInfo.pStages = shaderStages;
    //顶点输入状态
    pipelineCreateInfo.pVertexInputState = &inputStateCreateInfo;

    CALL_VK(vkCreateGraphicsPipelines(context.device, pipeline.cache_, 1, &pipelineCreateInfo,
                                      nullptr, &pipeline.pipeline_))
    //释放shader
    vkDestroyShaderModule(context.device,vertexShader, nullptr);
    vkDestroyShaderModule(context.device,fragmentShader, nullptr);
    return 0;
}

int VulkanFilter::createPipeline() {
    createPipeline(context.renderPass);
    return VK_SUCCESS;
}
