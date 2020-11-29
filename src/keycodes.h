#pragma once

#if defined(_WIN32)

#define KEY_ESCAPE VK_ESCAPE
#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_P 0x50


#elif defined(__linux__)

#define KEY_ESCAPE 0x9
#define KEY_W 0x19
#define KEY_A 0x26
#define KEY_S 0x27
#define KEY_D 0x28
#define KEY_P 0x21

#endif
