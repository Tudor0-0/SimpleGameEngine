#pragma once

#ifdef _WIN32
#include <windows.h>

inline void SetProcessDpiAwareness() {
    HMODULE user32 = LoadLibraryA("user32.dll");
    if (user32) {
        using SetProcessDpiAwarenessContextFunc = BOOL(WINAPI*)(HANDLE);
        auto SetProcessDpiAwarenessContext =
            reinterpret_cast<SetProcessDpiAwarenessContextFunc>(GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        if (SetProcessDpiAwarenessContext) {
            SetProcessDpiAwarenessContext(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4)));
        }
        FreeLibrary(user32);
    }
}
#endif