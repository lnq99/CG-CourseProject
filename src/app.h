#include "vulkanbase.h"

#include "Model/scene.h"
#include "control.h"

#define VERTEX_BUFFER_BIND_ID 0

#define TEX_DIM_W 1280
#define TEX_DIM_H 720

#define N_SPHERE 6


class VulkanApp : public VulkanBase
{
public:
    vks::Texture textureComputeTarget;
    Controller controller;
    Scene& scene;

    // Resources for the graphics part of the example
    struct {
        VkDescriptorSetLayout descriptorSetLayout;    // Raytraced image display shader binding layout
        VkDescriptorSet descriptorSetPreCompute;    // Raytraced image display shader bindings before compute shader image manipulation
        VkDescriptorSet descriptorSet;                // Raytraced image display shader bindings after compute shader image manipulation
        VkPipeline pipeline;                        // Raytraced image display pipeline
        VkPipelineLayout pipelineLayout;            // Layout of the graphics pipeline
    } graphics;

    // Resources for the compute part of the example
    struct {
        struct {
            vks::Buffer spheres;                        // (Shader) storage buffer object with scene spheres
            vks::Buffer planes;                        // (Shader) storage buffer object with scene planes
            vks::Buffer triangles;
        } storageBuffers;
        vks::Buffer stagingBuffer1;
        vks::Buffer stagingBuffer2;
        vks::Buffer stagingBuffer3;
        vks::Buffer uniformBuffer;                    // Uniform buffer object containing scene data
        VkQueue queue;                                // Separate queue for compute commands (queue family may differ from the one used for graphics)
        VkCommandPool commandPool;                    // Use a separate command pool (queue family may differ from the one used for graphics)
        VkCommandBuffer commandBuffer;                // Command buffer storing the dispatch commands and barriers
        VkFence fence;                                // Synchronization fence to avoid rewriting compute CB if still in use
        VkDescriptorSetLayout descriptorSetLayout;    // Compute shader binding layout
        VkDescriptorSet descriptorSet;                // Compute shader bindings
        VkPipelineLayout pipelineLayout;            // Layout of the compute pipeline
        VkPipeline pipeline;                        // Compute raytracing pipeline
    } compute;

    VulkanApp(void (*setupCamera)(Camera &), void (*createScene)(Scene &));

    ~VulkanApp();

    // Prepare a texture target that is used to store compute shader calculations
    void prepareTextureTarget(vks::Texture *tex, uint32_t width, uint32_t height, VkFormat format);

    void buildCommandBuffers();

    void buildComputeCommandBuffer();

    // Setup and fill the compute shader storage buffers containing primitives for the raytraced scene
    void prepareStorageBuffers();

    void updateSphere();

    void setupDescriptorPool();

    void setupDescriptorSetLayout();

    void setupDescriptorSet();

    void preparePipelines();

    // Prepare the compute pipeline that generates the ray traced image
    void prepareCompute();

    // Prepare and initialize uniform buffer containing shader uniforms
    void prepareUniformBuffers();

    void updateUniformBuffers();

    void draw();

    void prepare();

    virtual void render();

    virtual void viewChanged();

    virtual void OnUpdateUIOverlay() override;
};
