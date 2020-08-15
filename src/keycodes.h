#pragma once

#if defined(_WIN32)

#define KEY_ESCAPE VK_ESCAPE
#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_P 0x50
#define KEY_SPACE 0x20
#define KEY_KPADD 0x6B
#define KEY_KPSUB 0x6D

#define KEY_UP      VK_UP
#define KEY_LEFT    VK_LEFT
#define KEY_RIGHT   VK_RIGHT
#define KEY_DOWN    VK_DOWN
#define KEY_CTRL    VK_CONTROL


#elif defined(__linux__)

#define KEY_ESCAPE 0x9
#define KEY_W 0x19
#define KEY_A 0x26
#define KEY_S 0x27
#define KEY_D 0x28
#define KEY_P 0x21
#define KEY_SPACE 0x41
#define KEY_KPADD 0x56
#define KEY_KPSUB 0x52

#define KEY_UP          103
#define KEY_LEFT        105
#define KEY_RIGHT       106
#define KEY_DOWN        108
#define KEY_LEFTCTRL    29 + 8
#define KEY_RIGHTCTRL   97 + 8
#endif

// #include <linux/uinput.h>
// #include <linux/input-event-codes.h>
// https://docs.microsoft.com/vi-vn/windows/win32/inputdev/virtual-key-codes
// https://eastmanreference.com/complete-list-of-applescript-key-codes