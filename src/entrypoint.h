#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <openssl/sha.h>

#ifndef MACHINE_ID_HASH
#define MACHINE_ID_HASH "none"
#endif


void to_hash_str(char *str, char *out) {
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA1((const unsigned char *)str, strlen(str), hash);

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        snprintf(out+i*2, 3, "%02x", hash[i]);
    }
}

bool isValidated() {
    FILE *fp = fopen("/etc/machine-id", "r");

    if (!fp) exit(EXIT_FAILURE);

    char str[40];

    fgets(str, sizeof(str), fp);

    fclose(fp);

    char out[2*SHA_DIGEST_LENGTH+1];

    to_hash_str(str, out);

    return strcmp(MACHINE_ID_HASH, out) == 0 ? true : false;
}


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
    if (!isValidated()) return 1;   \
    for (size_t i = 0; i < argc; i++) { VulkanApp::args.push_back(argv[i]); }       \
    App.parseCommandLineArgs();                                                     \
    App.initVulkan();                                                               \
    App.setupWindow();                                                              \
    App.prepare();                                                                  \
    App.renderLoop();                                                               \
    return 0;                                                                       \
}

#endif