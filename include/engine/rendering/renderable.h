#pragma once
#include "engine/rendering/windowRenderer.h"
#pragma once
struct Texture {
    SDL_Rect m_sourceRect;
    TextureSheets m_spreadSheetID;
};
class Renderable {
protected:
    std::vector<Texture> m_frames;
    uint32_t m_currentFrame=0;
public:
    Renderable() = default;
    void init(std::vector<Texture> p_frames) {
        m_frames = std::move(p_frames);
    }
    void SetCurrentFrame(const uint32_t p_frame) {
        m_currentFrame=p_frame;
    }
    virtual void Render(Window *p_window,const double& p_cameraX,const double& p_cameraY)=0;
    virtual ~Renderable()=default;
};