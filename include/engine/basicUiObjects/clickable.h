#pragma once
#include "engine/spriteDataStuff/IUpdatable.h"
#include "engine/rendering/renderable.h"
#include "engine/utils/math.h"
class Clickable:public Renderable,public IUpdatable {
    protected:
    Transform m_transform;
    bool m_hovered  = false;
    bool m_held     = false;

    std::function<void()> m_OnClick={};
    std::function<void()> m_OnHover={};
    std::function<void()> m_OnHoverStop={};
    std::function<void()> m_OnClickRelease={};
    std::function<void()> m_OnClickCancel={};
    public:
    virtual ~Clickable()=default;
    void Init(const Transform& p_transform,
             std::vector<Texture> p_frames,
             std::function<void()> p_onHover = {},
             std::function<void()> p_onHoverStop={},
             std::function<void()> p_onClick = {},
             std::function<void()> p_onClickRelease={},
             std::function<void()> p_onClickCancel={}
            ) {
        Renderable::init(std::move(p_frames));
        m_transform = p_transform;
        m_OnClick   = std::move(p_onClick);
        m_OnHover   = std::move(p_onHover);
        m_OnHoverStop = std::move(p_onHoverStop);
        m_OnClickCancel = std::move(p_onClickCancel);
        m_OnClickRelease = std::move(p_onClickRelease);
    }

    virtual void OnMouseMoved(const float p_x, const float p_y, const double p_cameraX, const double p_cameraY) {
        const bool wasHovered = m_hovered;
        m_hovered = contains(p_x+p_cameraX, p_y+p_cameraY);
        if (m_hovered && !wasHovered) {
            if (m_OnHover) m_OnHover();
        }
        else if (!m_hovered && wasHovered) {
            if (m_held) {
                if (m_OnClickCancel) m_OnClickCancel();
                m_held = false;
            }
            if (m_OnHoverStop) m_OnHoverStop();
        }
    }

    virtual bool OnMousePressed() {
        if (m_hovered) {
            m_held = true;
            if (m_OnClick) {
                m_OnClick();
            }
            return true;
        }
        return false;
    }
    virtual bool OnMouseReleased() {
        if (m_held) {
            if (m_OnClickRelease) m_OnClickRelease();
            m_held = false;
            return true;
        }
        return false;
    }
    void Render(Window *window,const double &p_cameraX,const double &p_cameraY)override {
        window->RenderTexture(m_frames[m_currentFrame].m_spreadSheetID,
                                m_frames[m_currentFrame].m_sourceRect,
                                Scale(SDL_FRect(static_cast<float>(m_transform.x-p_cameraX),
                                                         static_cast<float>(m_transform.y-p_cameraY),
                                                         m_transform.width,
                                                         m_transform.height),
                                                         m_transform.scale),
                                m_transform.angle,m_transform.p_center,m_transform.p_flip
                                );
    };
    void Update(double dt)override{

    }
    void SetScale(const float &p_scale) {
        m_transform.scale = p_scale;
    }
    void Translate(double p_x,double p_y) {
        m_transform.x += p_x;
        m_transform.y += p_y;
    }
private:
    [[nodiscard]] bool contains(const double p_x, const double p_y) const {
        return p_x >= m_transform.x && p_x <= m_transform.x + m_transform.width
            && p_y >= m_transform.y && p_y <= m_transform.y + m_transform.height;
    }
};

