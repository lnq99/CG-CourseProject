/*
* UI overlay class using ImGui
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include <vulkan/vulkan.h>
#include "VulkanTools.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"

#include <glm/glm.hpp>

#include "imgui.h"


namespace vks 
{
    class UIOverlay 
    {
    public:
        vks::VulkanDevice *device;
        VkQueue queue;

        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        uint32_t subpass = 0;

        vks::Buffer vertexBuffer;
        vks::Buffer indexBuffer;
        int32_t vertexCount = 0;
        int32_t indexCount = 0;

        std::vector<VkPipelineShaderStageCreateInfo> shaders;

        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

        VkDeviceMemory fontMemory = VK_NULL_HANDLE;
        VkImage fontImage = VK_NULL_HANDLE;
        VkImageView fontView = VK_NULL_HANDLE;
        VkSampler sampler;

        struct PushConstBlock {
            glm::vec2 scale;
            glm::vec2 translate;
        } pushConstBlock;

        float scale = 1.0f;

        UIOverlay();
        ~UIOverlay();

        void preparePipeline(const VkPipelineCache pipelineCache, const VkRenderPass renderPass);
        void prepareResources();

        bool update();
        void draw(const VkCommandBuffer commandBuffer);
        void resize(uint32_t width, uint32_t height);

        void freeResources();
    };
}