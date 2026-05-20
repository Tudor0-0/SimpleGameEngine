#pragma once
#include <string>
#include <SDL.h>
#include <cstdint>

struct WindowSettings {
    //private:
    std::string m_title;
    int32_t     posx = SDL_WINDOWPOS_CENTERED;
    int32_t     posy = SDL_WINDOWPOS_CENTERED;
    int32_t     width = 1600;
    int32_t     height = 900;
    uint32_t    targetFps = 144;
    uint32_t    windowFlags = SDL_WINDOW_SHOWN|SDL_WINDOW_BORDERLESS;
    uint32_t    rendererFlags = SDL_RENDERER_ACCELERATED;
    WindowSettings() = default;
    WindowSettings(const WindowSettings& p_windowSettings) {
        m_title = p_windowSettings.m_title;
        posx = p_windowSettings.posx;
        posy = p_windowSettings.posy;
        width = p_windowSettings.width;
        height = p_windowSettings.height;
        targetFps = p_windowSettings.targetFps;
        windowFlags = p_windowSettings.windowFlags;
        rendererFlags = p_windowSettings.rendererFlags;
    }
    WindowSettings(WindowSettings&& p_windowSettings)  noexcept {
        m_title = std::move(p_windowSettings.m_title);
        posx = p_windowSettings.posx;
        posy = p_windowSettings.posy;
        width = p_windowSettings.width;
        height = p_windowSettings.height;
        windowFlags = p_windowSettings.windowFlags;
        rendererFlags = p_windowSettings.rendererFlags;

    }
};