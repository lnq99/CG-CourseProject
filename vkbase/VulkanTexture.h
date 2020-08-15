/*
* Vulkan texture loader
*/

#pragma once

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"

#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanTools.h"


namespace vks
{
    class Texture
    {
    public:
        vks::VulkanDevice *   device;
        VkImage               image;
        VkImageLayout         imageLayout;
        VkDeviceMemory        deviceMemory;
        VkImageView           view;
        uint32_t              width, height;
        uint32_t              mipLevels;
        uint32_t              layerCount;
        VkDescriptorImageInfo descriptor;
        VkSampler             sampler;

        void    updateDescriptor();
        void    destroy();
    };
}        // namespace vks
