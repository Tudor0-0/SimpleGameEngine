#pragma once
#include "SDL.h"
#include "engine/utils/windowSettings.h"
#include "engine/utils/windowsRescale.h"
#include "engine/core/event.h"
#include <bitset>
#include <functional>

///modify only here
///     ||
///     v
enum class TextureSheets :uint32_t{
    testButtons,wControlButtons,star,maxTextureCount
};
inline constexpr const char* TEXTURE_PATHS[] = {
    "assets/testbuttons.png","assets/wControl.png","assets/star.png"
};
///
///
///
class Window {
private:
    SDL_Window*       m_window    =  nullptr;
    SDL_Renderer*     m_renderer  =  nullptr;
    bool              m_fullscreen  =  true;
    uint8_t           m_keyState[SDL_NUM_SCANCODES]   =  {};
    uint32_t          m_mouseState = 0;
    float            m_mouseX = 0;
    float            m_mouseY = 0;
    std::vector<SDL_Texture*> m_textures;
    //std::bitset<256>  keyBoardState; sdl e retardat
    //std::bitset<8>    mouseState;
    std::function<void(const Event& )> m_eventSender;

public:
    explicit Window(const WindowSettings& p_config=WindowSettings());
    virtual ~Window();

    // event handlers
    void BuildTextures();
    void SetEventHandler(std::function<void(const Event&)> p_eventSender);
    void GetInput();

    bool RenderTexture(TextureSheets p_textureSheetId, const SDL_Rect &p_srcRect, const SDL_FRect &p_dstRect, const double &p_angle=0, const
                       SDL_FPoint &p_center={0.0,0.0}, SDL_RendererFlip p_flip= SDL_FLIP_NONE) const;

    bool LoadTexture(SDL_Texture *&p_dest, const char *p_path) const;

    void RenderClear(uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=255) const;

    void RenderPresent() const;

    void SetResolutionAndScaling(int32_t p_windowWidth, int32_t p_windowHeight, int32_t p_internalWidth, int32_t p_internalHeight) const;

    void Render() const;

    void RenderRect(SDL_Rect p_rect={ 0,0,1600,32 },uint8_t r=50,uint8_t g=50,uint8_t b=50,uint8_t a=20) const;

    void Minimize() const;

    double GetTime();
};

