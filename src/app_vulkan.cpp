#include "app.h"

#define LOCAL_SIZE 16

VulkanApp::VulkanApp(void (*setupCamera)(Camera &), void (*createScene)(Scene &))
    : VulkanBase()
    , scene(Scene::instance())
{
    title = "Compute shader - ray tracing";

    scene.ubo.lightPos = { 0, 3.8, 0 };
    scene.ubo.aspectRatio = (float)width / (float)height;
    timerSpeed *= 0.8f;

    setupCamera(camera);
    createScene(scene);
}


VulkanApp::~VulkanApp()
{
    // Graphics
    vkDestroyPipeline(device, graphics.pipeline, nullptr);
    vkDestroyPipelineLayout(device, graphics.pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, graphics.descriptorSetLayout, nullptr);

    // Compute
    vkDestroyPipeline(device, compute.pipeline, nullptr);
    vkDestroyPipelineLayout(device, compute.pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, compute.descriptorSetLayout, nullptr);
    vkDestroyFence(device, compute.fence, nullptr);
    vkDestroyCommandPool(device, compute.commandPool, nullptr);

    compute.uniformBuffer.destroy();
    compute.storageBuffers.spheres.destroy();
    compute.storageBuffers.planes.destroy();
    compute.storageBuffers.triangles.destroy();
    compute.stagingBuffer1.destroy();
    compute.stagingBuffer2.destroy();
    compute.stagingBuffer3.destroy();

    textureComputeTarget.destroy();
}


// Prepare a texture target that is used to store compute shader calculations
void VulkanApp::prepareTextureTarget(vks::Texture *tex, uint32_t width, uint32_t height, VkFormat format)
{
    // Get device properties for the requested texture format
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
    // Check if requested image format supports image storage operations
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

    // Prepare blit target texture
    tex->width = width;
    tex->height = height;

    VkImageCreateInfo imageCreateInfo = vks::initializers::imageCreateInfo();
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.extent = { width, height, 1 };
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // Image will be sampled in the fragment shader and used as storage target in the compute shader
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    imageCreateInfo.flags = 0;

    VkMemoryAllocateInfo memAllocInfo = vks::initializers::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &tex->image));
    vkGetImageMemoryRequirements(device, tex->image, &memReqs);
    memAllocInfo.allocationSize = memReqs.size;
    memAllocInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &tex->deviceMemory));
    VK_CHECK_RESULT(vkBindImageMemory(device, tex->image, tex->deviceMemory, 0));

    VkCommandBuffer layoutCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    tex->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    vks::tools::setImageLayout(
        layoutCmd,
        tex->image,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        tex->imageLayout);

    vulkanDevice->flushCommandBuffer(layoutCmd, queue, true);

    // Create sampler
    VkSamplerCreateInfo sampler = vks::initializers::samplerCreateInfo();
    sampler.magFilter = VK_FILTER_LINEAR;
    sampler.minFilter = VK_FILTER_LINEAR;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler.addressModeV = sampler.addressModeU;
    sampler.addressModeW = sampler.addressModeU;
    sampler.mipLodBias = 0.0f;
    sampler.maxAnisotropy = 1.0f;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = 0.0f;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    VK_CHECK_RESULT(vkCreateSampler(device, &sampler, nullptr, &tex->sampler));

    // Create image view
    VkImageViewCreateInfo view = vks::initializers::imageViewCreateInfo();
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = format;
    view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    view.image = tex->image;
    VK_CHECK_RESULT(vkCreateImageView(device, &view, nullptr, &tex->view));

    // Initialize a descriptor for later use
    tex->descriptor.imageLayout = tex->imageLayout;
    tex->descriptor.imageView = tex->view;
    tex->descriptor.sampler = tex->sampler;
    tex->device = vulkanDevice;
}


void VulkanApp::buildCommandBuffers()
{
    VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

    VkClearValue clearValues[2];
    clearValues[0].color = defaultClearColor;
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = vks::initializers::renderPassBeginInfo();
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
        // Set target frame buffer
        renderPassBeginInfo.framebuffer = frameBuffers[i];

        VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));

        // Image memory barrier to make sure that compute shader writes are finished before sampling from the texture
        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.image = textureComputeTarget.image;
        imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(
            drawCmdBuffers[i],
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_FLAGS_NONE,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);

        vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = vks::initializers::viewport((float)width, (float)height, 0.0f, 1.0f);
        vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

        VkRect2D scissor = vks::initializers::rect2D(width, height, 0, 0);
        vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

        // Display ray traced image generated by compute shader as a full screen quad
        // Quad vertices are generated in the vertex shader
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipelineLayout, 0, 1, &graphics.descriptorSet, 0, NULL);
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipeline);
        vkCmdDraw(drawCmdBuffers[i], 3, 1, 0, 0);

        drawUI(drawCmdBuffers[i]);

        vkCmdEndRenderPass(drawCmdBuffers[i]);

        VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
    }
}


void VulkanApp::buildComputeCommandBuffer()
{
    VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

    VK_CHECK_RESULT(vkBeginCommandBuffer(compute.commandBuffer, &cmdBufInfo));

    vkCmdBindPipeline(compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipeline);
    vkCmdBindDescriptorSets(compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipelineLayout, 0, 1, &compute.descriptorSet, 0, 0);

    vkCmdDispatch(compute.commandBuffer, textureComputeTarget.width / LOCAL_SIZE, textureComputeTarget.height / LOCAL_SIZE, 1);

    vkEndCommandBuffer(compute.commandBuffer);
}


void VulkanApp::setupDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo =
        vks::initializers::descriptorSetAllocateInfo(
            descriptorPool,
            &graphics.descriptorSetLayout,
            1);

    VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &graphics.descriptorSet));

    std::vector<VkWriteDescriptorSet> writeDescriptorSets =
    {
        // Binding 0 : Fragment shader texture sampler
        vks::initializers::writeDescriptorSet(
            graphics.descriptorSet,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            0,
            &textureComputeTarget.descriptor)
    };

    vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);
}

void VulkanApp::setupDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes =
    {
        vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),            // Compute UBO
        vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4),    // Graphics image samplers
        vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1),                // Storage image for ray traced image output
        vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),            // Storage buffer for the scene primitives
    };

    VkDescriptorPoolCreateInfo descriptorPoolInfo =
        vks::initializers::descriptorPoolCreateInfo(
            poolSizes.size(),
            poolSizes.data(),
            3);

    VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool));
}

// Prepare the compute pipeline that generates the ray traced image
void VulkanApp::prepareCompute()
{
    // Create a compute capable device queue
    // The VulkanDevice::createLogicalDevice functions finds a compute capable queue and prefers queue families that only support compute
    // Depending on the implementation this may result in different queue family indices for graphics and computes,
    // requiring proper synchronization (see the memory barriers in buildComputeCommandBuffer)
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = NULL;
    queueCreateInfo.queueFamilyIndex = vulkanDevice->queueFamilyIndices.compute;
    queueCreateInfo.queueCount = 1;
    vkGetDeviceQueue(device, vulkanDevice->queueFamilyIndices.compute, 0, &compute.queue);

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        // Binding 0: Storage image (raytraced output)
        vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0),
        // Binding 1: Uniform buffer block
        vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_COMPUTE_BIT,
            1),
        // Binding 2: Shader storage buffer for the spheres
        vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            VK_SHADER_STAGE_COMPUTE_BIT,
            2),
        // Binding 3: Shader storage buffer for the planes
        vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            VK_SHADER_STAGE_COMPUTE_BIT,
            3),
        // Binding 4: Shader storage buffer for the triangles
        vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            VK_SHADER_STAGE_COMPUTE_BIT,
            4)
    };

    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        vks::initializers::descriptorSetLayoutCreateInfo(
            setLayoutBindings.data(),
            setLayoutBindings.size());

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr,    &compute.descriptorSetLayout));

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
        vks::initializers::pipelineLayoutCreateInfo(
            &compute.descriptorSetLayout,
            1);

    VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &compute.pipelineLayout));

    VkDescriptorSetAllocateInfo allocInfo =
        vks::initializers::descriptorSetAllocateInfo(
            descriptorPool,
            &compute.descriptorSetLayout,
            1);

    VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &compute.descriptorSet));

    std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets =
    {
        // Binding 0: Output storage image
        vks::initializers::writeDescriptorSet(
            compute.descriptorSet,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            0,
            &textureComputeTarget.descriptor),
        // Binding 1: Uniform buffer block
        vks::initializers::writeDescriptorSet(
            compute.descriptorSet,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1,
            &compute.uniformBuffer.descriptor),
        // Binding 2: Shader storage buffer for the spheres
        vks::initializers::writeDescriptorSet(
            compute.descriptorSet,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            2,
            &compute.storageBuffers.spheres.descriptor),
        // Binding 3: Shader storage buffer for the planes
        vks::initializers::writeDescriptorSet(
            compute.descriptorSet,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            3,
            &compute.storageBuffers.planes.descriptor),
        // Binding 4: Shader storage buffer for the triangles
        vks::initializers::writeDescriptorSet(
            compute.descriptorSet,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            4,
            &compute.storageBuffers.triangles.descriptor)
    };

    vkUpdateDescriptorSets(device, computeWriteDescriptorSets.size(), computeWriteDescriptorSets.data(), 0, NULL);

    // Create compute shader pipelines
    VkComputePipelineCreateInfo computePipelineCreateInfo =
        vks::initializers::computePipelineCreateInfo(
            compute.pipelineLayout,
            0);

    computePipelineCreateInfo.stage = loadShader(getShadersPath() + "raytracing.comp.spv", VK_SHADER_STAGE_COMPUTE_BIT);
    VK_CHECK_RESULT(vkCreateComputePipelines(device, pipelineCache, 1, &computePipelineCreateInfo, nullptr, &compute.pipeline));

    // Separate command pool as queue family for compute may be different than graphics
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = vulkanDevice->queueFamilyIndices.compute;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK_RESULT(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &compute.commandPool));

    // Create a command buffer for compute operations
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        vks::initializers::commandBufferAllocateInfo(
            compute.commandPool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            1);

    VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &compute.commandBuffer));

    // Fence for compute CB sync
    VkFenceCreateInfo fenceCreateInfo = vks::initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &compute.fence));

    // Build a single command buffer containing the compute dispatch commands
    buildComputeCommandBuffer();
}

// Prepare and initialize uniform buffer containing shader uniforms
void VulkanApp::prepareUniformBuffers()
{
    // Compute shader parameter uniform buffer block
    vulkanDevice->createBuffer(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &compute.uniformBuffer,
        sizeof(scene.ubo));

    updateUniformBuffers();
}

void VulkanApp::setupDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
    {
        // Binding 0 : Fragment shader image sampler
        vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            0)
    };

    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        vks::initializers::descriptorSetLayoutCreateInfo(
            setLayoutBindings.data(),
            setLayoutBindings.size());

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &graphics.descriptorSetLayout));

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
        vks::initializers::pipelineLayoutCreateInfo(
            &graphics.descriptorSetLayout,
            1);

    VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &graphics.pipelineLayout));
}

void VulkanApp::preparePipelines()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        vks::initializers::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            0,
            VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        vks::initializers::pipelineRasterizationStateCreateInfo(
            VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_FRONT_BIT,
            VK_FRONT_FACE_COUNTER_CLOCKWISE,
            0);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        vks::initializers::pipelineColorBlendAttachmentState(
            0xf,
            VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        vks::initializers::pipelineColorBlendStateCreateInfo(
            1,
            &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        vks::initializers::pipelineDepthStencilStateCreateInfo(
            VK_FALSE,
            VK_FALSE,
            VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        vks::initializers::pipelineViewportStateCreateInfo(1, 1, 0);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        vks::initializers::pipelineMultisampleStateCreateInfo(
            VK_SAMPLE_COUNT_1_BIT,
            0);

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        vks::initializers::pipelineDynamicStateCreateInfo(
            dynamicStateEnables.data(),
            dynamicStateEnables.size(),
            0);

    // Display pipeline
    VkPipelineShaderStageCreateInfo shaderStages[2];

    shaderStages[0] = loadShader(getShadersPath() + "texture.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = loadShader(getShadersPath() + "texture.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        vks::initializers::pipelineCreateInfo(
            graphics.pipelineLayout,
            renderPass,
            0);

    VkPipelineVertexInputStateCreateInfo emptyInputState{};
    emptyInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    emptyInputState.vertexAttributeDescriptionCount = 0;
    emptyInputState.pVertexAttributeDescriptions = nullptr;
    emptyInputState.vertexBindingDescriptionCount = 0;
    emptyInputState.pVertexBindingDescriptions = nullptr;
    pipelineCreateInfo.pVertexInputState = &emptyInputState;

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.renderPass = renderPass;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &graphics.pipeline));
}

// Setup and fill the compute shader storage buffers containing primitives for the raytraced scene
void VulkanApp::prepareStorageBuffers()
{
    VkDeviceSize storageBufferSize = scene.spheres.size() * sizeof(Sphere);

    vulkanDevice->createBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &compute.stagingBuffer1,
        storageBufferSize,
        scene.spheres.data());

    // updateSphere();

    vulkanDevice->createBuffer(
        // The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &compute.storageBuffers.spheres,
        N_SPHERE * sizeof(Sphere));

    // Copy to staging buffer
    VkCommandBuffer copyCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    VkBufferCopy copyRegion = {};
    copyRegion.size = storageBufferSize;
    vkCmdCopyBuffer(copyCmd, compute.stagingBuffer1.buffer, compute.storageBuffers.spheres.buffer, 1, &copyRegion);
    vulkanDevice->flushCommandBuffer(copyCmd, queue, true);

    storageBufferSize = scene.planes.size() * sizeof(Plane);

    // Stage
    vulkanDevice->createBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &compute.stagingBuffer2,
        storageBufferSize,
        scene.planes.data());

    vulkanDevice->createBuffer(
        // The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &compute.storageBuffers.planes,
        storageBufferSize);

    // Copy to staging buffer
    copyCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    copyRegion.size = storageBufferSize;
    vkCmdCopyBuffer(copyCmd, compute.stagingBuffer2.buffer, compute.storageBuffers.planes.buffer, 1, &copyRegion);
    vulkanDevice->flushCommandBuffer(copyCmd, queue, true);


    storageBufferSize = scene.triangles.size() * sizeof(Triangle);

    // Stage
    vulkanDevice->createBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &compute.stagingBuffer3,
        storageBufferSize,
        scene.triangles.data());

    vulkanDevice->createBuffer(
        // The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &compute.storageBuffers.triangles,
        storageBufferSize);

    // Copy to staging buffer
    copyCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    copyRegion.size = storageBufferSize;
    vkCmdCopyBuffer(copyCmd, compute.stagingBuffer3.buffer, compute.storageBuffers.triangles.buffer, 1, &copyRegion);
    vulkanDevice->flushCommandBuffer(copyCmd, queue, true);
}

void VulkanApp::updateUniformBuffers()
{
    scene.ubo.lightPos.x = 0.0f + sin(glm::radians(timer * 360.0f)) * cos(glm::radians(timer * 360.0f)) * 2.0f;
    scene.ubo.lightPos.y = 0.0f + sin(glm::radians(timer * 360.0f)) * 2.0f;
    scene.ubo.lightPos.z = 0.0f + cos(glm::radians(timer * 360.0f)) * 2.0f;
    scene.ubo.camera.pos = camera.position * -1.0f;
    
    // scene.ubo.n = scene.spheres.size();

    VK_CHECK_RESULT(compute.uniformBuffer.map());
    memcpy(compute.uniformBuffer.mapped, &scene.ubo, sizeof(scene.ubo));
    compute.uniformBuffer.unmap();
}

void VulkanApp::draw()
{
    VulkanBase::prepareFrame();

    // Command buffer to be submitted to the queue
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];
    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    VulkanBase::submitFrame();

    // Submit compute commands
    // Use a fence to ensure that compute command buffer has finished executing before using it again
    vkWaitForFences(device, 1, &compute.fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &compute.fence);

    VkSubmitInfo computeSubmitInfo = vks::initializers::submitInfo();
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &compute.commandBuffer;

    VK_CHECK_RESULT(vkQueueSubmit(compute.queue, 1, &computeSubmitInfo, compute.fence));
}

void VulkanApp::prepare()
{
    VulkanBase::prepare();
    prepareStorageBuffers();
    prepareUniformBuffers();
    prepareTextureTarget(&textureComputeTarget, TEX_DIM_W, TEX_DIM_H, VK_FORMAT_R8G8B8A8_UNORM);
    setupDescriptorSetLayout();
    preparePipelines();
    setupDescriptorPool();
    setupDescriptorSet();
    prepareCompute();
    buildCommandBuffers();
    prepared = true;
}

void VulkanApp::render()
{
    if (!prepared)
        return;
    draw();
    if (!paused)
    {
        updateSphere();
        updateUniformBuffers();
    }
}

void VulkanApp::viewChanged()
{
    scene.ubo.aspectRatio = (float)width / (float)height;
    updateUniformBuffers();
}

void VulkanApp::updateSphere()
{
    auto nSphere = scene.spheres.size();
    if (scene.selected < nSphere)
        scene.spheres[scene.selected].pos = controller.translate;

    VK_CHECK_RESULT(compute.stagingBuffer1.map());
    memcpy(compute.stagingBuffer1.mapped, scene.spheres.data(), nSphere * sizeof(Sphere));
    compute.stagingBuffer1.unmap();

    VkDeviceSize storageBufferSize = nSphere * sizeof(Sphere);

    // Copy to staging buffer
    VkCommandBuffer copyCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    VkBufferCopy copyRegion = {};
    copyRegion.size = storageBufferSize;
    vkCmdCopyBuffer(copyCmd, compute.stagingBuffer1.buffer, compute.storageBuffers.spheres.buffer, 1, &copyRegion);
    vulkanDevice->flushCommandBuffer(copyCmd, queue, true);
}
