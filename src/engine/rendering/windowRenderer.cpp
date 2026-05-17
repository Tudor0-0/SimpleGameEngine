#include "engine/rendering/windowRenderer.h"
#include "engine/utils/exceptions.h"
#include <iostream>
#include <SDL_image.h>
#include <cstring>
Window::Window(const WindowSettings &p_config) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        THROW_SDL_INIT_ERROR(std::string("Failed to initialize SDL: ") + SDL_GetError());
    }

    m_window = SDL_CreateWindow(
        p_config.m_title.c_str(),
        p_config.posx,
        p_config.posy,
        p_config.width,
        p_config.height,
        p_config.windowFlags
    );
    if (!m_window) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
        THROW_SDL_INIT_ERROR(std::string("Failed to initialize the Window: ") + SDL_GetError());
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, p_config.rendererFlags);
    if (!m_renderer) {
        SDL_DestroyWindow(m_window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
        THROW_SDL_INIT_ERROR(std::string("Failed to initialize the Renderer: ") + SDL_GetError());
    }
    m_textures.resize(static_cast<uint32_t>(TextureSheets::maxTextureCount));
    SetResolutionAndScaling(p_config.width, p_config.height, p_config.width, p_config.height);
    BuildTextures();
}

Window::~Window() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Window::RenderTexture(TextureSheets p_textureSheetId,
                           const SDL_Rect &p_srcRect,
                           const SDL_FRect &p_dstRect,
                           const double &p_angle,
                           const SDL_FPoint &p_center,
                           const SDL_RendererFlip p_flip ) const
{
    if (const int32_t result = SDL_RenderCopyExF(m_renderer,
                                                m_textures[static_cast<uint32_t>(p_textureSheetId)],
                                                &p_srcRect,
                                                &p_dstRect,
                                                p_angle,
                                                &p_center,
                                                p_flip); result != 0) {
        std::cerr << "Texture rendering failed: " << SDL_GetError() << '\n';
        return false;
                                                }

    return true;
}
bool Window::LoadTexture(SDL_Texture*& p_dest, const char* p_path) const {
    SDL_Surface* temps = IMG_Load(p_path);
    if (temps == nullptr)
    {
        std::cerr<<"Failed to load surface from path"<<p_path<<" SDL_image Error: "<< IMG_GetError()<<'\n';
        p_dest = nullptr;
        return false;
    }
    p_dest = SDL_CreateTextureFromSurface(m_renderer, temps);
    SDL_FreeSurface(temps);

    if (p_dest == nullptr)
    {
        std::cerr<<"Failed to create texture from"<<p_path<<" SDL Error: "<< SDL_GetError()<<'\n';
        return false;
    }

    return true;
}
void Window::RenderClear(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    SDL_RenderClear(m_renderer);
}
void Window::RenderPresent() const {
    SDL_RenderPresent(m_renderer);
}
void Window::SetResolutionAndScaling(const int32_t p_windowWidth, const int32_t p_windowHeight, const int32_t p_internalWidth, const int32_t p_internalHeight) const {
    for (int i=1;i<=5;i++) {
        SDL_RenderPresent(m_renderer);
        SDL_RenderClear(m_renderer);
    }
    SDL_SetWindowSize(m_window, p_windowWidth, p_windowHeight);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_RenderSetLogicalSize(m_renderer, p_internalWidth, p_internalHeight);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");


}

void Window::Render() const {
    RenderPresent();
}
double Window::GetTime() {
    const uint64_t count = SDL_GetPerformanceCounter();
    const uint64_t freq = SDL_GetPerformanceFrequency();
    return static_cast<double>(count) / static_cast<double>(freq);
}
void Window::BuildTextures() {
    for (uint32_t i = 0; i < static_cast<uint32_t>(TextureSheets::maxTextureCount); i++) {
       LoadTexture(m_textures[i], TEXTURE_PATHS[i]);
    }
}

void Window::RenderRect(SDL_Rect p_rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderFillRect(m_renderer, &p_rect);

}
void Window::Minimize() const {
    SDL_MinimizeWindow(m_window);
}
