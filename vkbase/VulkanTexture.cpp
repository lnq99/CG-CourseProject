/*
* Vulkan texture loader
*/

#include <VulkanTexture.h>

namespace vks
{
    void Texture::updateDescriptor()
    {
        descriptor.sampler = sampler;
        descriptor.imageView = view;
        descriptor.imageLayout = imageLayout;
    }

    void Texture::destroy()
    {
        vkDestroyImageView(device->logicalDevice, view, nullptr);
        vkDestroyImage(device->logicalDevice, image, nullptr);
        if (sampler)
        {
            vkDestroySampler(device->logicalDevice, sampler, nullptr);
        }
        vkFreeMemory(device->logicalDevice, deviceMemory, nullptr);
    }
}
