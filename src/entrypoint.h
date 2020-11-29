#if defined(_WIN32)

#define VULKAN_MAIN()                                                               \
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)        \
{                                                                                   \
    App.handleMessages(hWnd, uMsg, wParam, lParam);                                 \
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));                             \
}                                                                                   \
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)                    \
{                                                                                   \
    for (int32_t i = 0; i < __argc; i++) { VulkanApp::args.push_back(__argv[i]); }  \
    App.parseCommandLineArgs();                                                     \
    App.initVulkan();                                                               \
    App.setupWindow(hInstance, WndProc);                                            \
    App.prepare();                                                                  \
    App.renderLoop();                                                               \
    return 0;                                                                       \
}


#elif defined(VK_USE_PLATFORM_XCB_KHR)

#define VULKAN_MAIN()                                                               \
static void handleEvent(const xcb_generic_event_t *event)                           \
{                                                                                   \
    App.handleEvent(event);                                                         \
}                                                                                   \
int main(const int argc, const char *argv[])                                        \
{                                                                                   \
    for (size_t i = 0; i < argc; i++) { VulkanApp::args.push_back(argv[i]); }       \
    App.parseCommandLineArgs();                                                     \
    App.initVulkan();                                                               \
    App.setupWindow();                                                              \
    App.prepare();                                                                  \
    App.renderLoop();                                                               \
    return 0;                                                                       \
}

#endif