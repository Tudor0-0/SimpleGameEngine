#pragma once
#include <string>
#include <SDL.h>
#include <cstdint>

struct WindowSettings {
    // private:
    std::string m_title;
    int32_t posx = SDL_WINDOWPOS_CENTERED;
    int32_t posy = SDL_WINDOWPOS_CENTERED;
    int32_t width = 1600;
    int32_t height = 900;
    uint32_t targetFps = 144;
    uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE;
    uint32_t rendererFlags = SDL_RENDERER_ACCELERATED;
};