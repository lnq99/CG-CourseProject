#pragma once

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingAPI.h>
#elif defined(VK_USE_PLATFORM_XCB_KHR)
#include <xcb/xcb.h>
#endif

#include <iostream>
#include <chrono>
#include <sys/stat.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/matrix_inverse.hpp>
// #include <glm/gtc/type_ptr.hpp>

#include "vulkan/vulkan.h"

#include "keycodes.h"
#include "VulkanTools.h"
#include "VulkanUIOverlay.h"
#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"

#include "VulkanInitializers.hpp"
#include "Model/camera.h"

class VulkanBase
{
private:
    bool viewUpdated = false;
    bool resizing = false;
    uint32_t destWidth;
    uint32_t destHeight;

    void windowResize();
    void handleMouseMove(int32_t x, int32_t y);
    void nextFrame();
    void updateOverlay();
    void createPipelineCache();
    void createCommandPool();
    void createSynchronizationPrimitives();
    void initSwapchain();
    void setupSwapChain();
    void createCommandBuffers();
    void destroyCommandBuffers();

protected:
    std::string getShadersPath() const;

    // Frame counter to display fps
    uint32_t frameCounter = 0;
    uint32_t lastFPS = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;
    // Vulkan instance, stores all per-application states
    VkInstance instance;
    // Physical device (GPU) that Vulkan will use
    VkPhysicalDevice physicalDevice;
    // Stores physical device properties (for e.g. checking device limits)
    VkPhysicalDeviceProperties deviceProperties;
    // Stores the features available on the selected physical device (for e.g. checking if a feature is available)
    VkPhysicalDeviceFeatures deviceFeatures;
    // Stores all available memory (type) properties for the physical device
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    /** @brief Set of physical device features to be enabled (must be set in the derived constructor) */
    VkPhysicalDeviceFeatures enabledFeatures{};
    /** @brief Set of device extensions to be enabled (must be set in the derived constructor) */
    std::vector<const char*> enabledDeviceExtensions;
    std::vector<const char*> enabledInstanceExtensions;
    /** @brief Optional pNext structure for passing extension structures to device creation */
    void* deviceCreatepNextChain = nullptr;
    /** @brief Logical device, application's view of the physical device (GPU) */
    VkDevice device;
    // Handle to the device graphics queue that command buffers are submitted to
    VkQueue queue;
    // Depth buffer format (selected during Vulkan initialization)
    VkFormat depthFormat;
    // Command buffer pool
    VkCommandPool cmdPool;
    /** @brief Pipeline stages used to wait at for graphics queue submissions */
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Contains command buffers and semaphores to be presented to the queue
    VkSubmitInfo submitInfo;
    // Command buffers used for rendering
    std::vector<VkCommandBuffer> drawCmdBuffers;
    // Global render pass for frame buffer writes
    VkRenderPass renderPass;
    // List of available frame buffers (same as number of swap chain images)
    std::vector<VkFramebuffer>frameBuffers;
    // Active frame buffer index
    uint32_t currentBuffer = 0;
    // Descriptor set pool
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    // List of shader modules created (stored for cleanup)
    std::vector<VkShaderModule> shaderModules;
    // Pipeline cache object
    VkPipelineCache pipelineCache;
    // Wraps the swap chain to present images (framebuffers) to the windowing system
    VulkanSwapChain swapChain;
    // Synchronization semaphores
    struct {
        // Swap chain image presentation
        VkSemaphore presentComplete;
        // Command buffer submission and execution
        VkSemaphore renderComplete;
    } semaphores;
    std::vector<VkFence> waitFences;

public:
    bool prepared = false;
    uint32_t width = 1280;
    uint32_t height = 720;

    vks::UIOverlay UIOverlay;

    /** @brief Last frame time measured using a high performance timer (if available) */
    float frameTimer = 1.0f;

    /** @brief Encapsulated physical and logical vulkan device */
    vks::VulkanDevice *vulkanDevice;

    struct Settings {
        bool fullscreen = false;
        bool vsync = false;
        bool discrete = true;
    } settings;

    VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };

    static std::vector<const char*> args;

    // Defines a frame rate independent timer value clamped from -1.0...1.0
    // For use in animations, rotations, etc.
    float timer = 0.0f;
    // Multiplier for speeding up (or slowing down) the global timer
    float timerSpeed = 0.2f;
    bool paused = false;

    Camera camera;
    glm::vec2 mousePos;

    std::string title = "Vulkan App";
    std::string name = "vulkanApp";
    uint32_t apiVersion = VK_API_VERSION_1_2;

    struct {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depthStencil;

    struct {
        bool left = false;
        bool right = false;
        bool middle = false;
    } mouseButtons;

    // OS specific
#if defined(_WIN32)
    HWND window;
    HINSTANCE windowInstance;
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    bool quit = false;
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif

    VulkanBase();
    virtual ~VulkanBase();
    void parseCommandLineArgs();
    /** @brief Setup the vulkan instance, enable required extensions and connect to the physical device (GPU) */
    bool initVulkan();

#if defined(_WIN32)
    void setupDPIAwareness();
    HWND setupWindow(HINSTANCE hinstance, WNDPROC wndproc);
    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    xcb_window_t setupWindow();
    void initxcbConnection();
    void handleEvent(const xcb_generic_event_t *event);
#endif
    /** @brief (Virtual) Creates the application wide Vulkan instance */
    virtual VkResult createInstance();
    /** @brief (Pure virtual) Render function to be implemented by the sample application */
    virtual void render() = 0;
    /** @brief (Virtual) Called when the camera view has changed */
    virtual void viewChanged();
    /** @brief (Virtual) Called after a key was pressed, can be used to do custom key handling */
    virtual void keyPressed(uint32_t);
    /** @brief (Virtual) Called after the mouse cursor moved and before internal events (like camera rotation) is handled */
    virtual void mouseMoved(double x, double y, bool &handled);
    /** @brief (Virtual) Called when the window has been resized, can be used by the sample application to recreate resources */
    virtual void windowResized();
    /** @brief (Virtual) Called when resources have been recreated that require a rebuild of the command buffers (e.g. frame buffer), to be implemented by the sample application */
    virtual void buildCommandBuffers();
    /** @brief (Virtual) Setup default depth and stencil views */
    virtual void setupDepthStencil();
    /** @brief (Virtual) Setup default framebuffers for all requested swapchain images */
    virtual void setupFrameBuffer();
    /** @brief (Virtual) Setup a default renderpass */
    virtual void setupRenderPass();

    /** @brief Prepares all Vulkan resources and functions required to run the sample */
    virtual void prepare();

    /** @brief Loads a SPIR-V shader file for the given shader stage */
    VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

    /** @brief Entry point for the main render loop */
    void renderLoop();

    /** @brief Adds the drawing commands for the ImGui overlay to the given command buffer */
    void drawUI(const VkCommandBuffer commandBuffer);

    /** Prepare the next frame for workload submission by acquiring the next swap chain image */
    void prepareFrame();
    /** @brief Presents the current image to the swap chain */
    void submitFrame();
    /** @brief (Virtual) Default image acquire + submission and command buffer submission function */
    virtual void renderFrame();

    /** @brief (Virtual) Called when the UI overlay is updating, can be used to add custom elements to the overlay */
    virtual void OnUpdateUIOverlay();
};
