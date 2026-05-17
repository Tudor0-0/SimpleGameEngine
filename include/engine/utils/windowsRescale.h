#pragma once
///windows ffs
///
#ifdef _WIN32
#include <windows.h>
void SetProcessDpiAwareness() {
    HMODULE user32 = LoadLibraryA("user32.dll");
    if (user32) {
        typedef BOOL(WINAPI* SetProcessDpiAwarenessContextFunc)(HANDLE);
        SetProcessDpiAwarenessContextFunc SetProcessDpiAwarenessContext =
            (SetProcessDpiAwarenessContextFunc)GetProcAddress(user32, "SetProcessDpiAwarenessContext");//raw pointer from "getprocessadress typecasted
        if (SetProcessDpiAwarenessContext) {
            SetProcessDpiAwarenessContext((HANDLE)(INT_PTR)-4);
            FreeLibrary(user32);
        }
        FreeLibrary(user32);
    }
}
#endif