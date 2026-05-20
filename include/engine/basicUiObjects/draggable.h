#pragma once
#include "engine/basicUiObjects/clickable.h"
//Work in progress buggy
class Draggable : public Clickable {
protected:
    double m_dragOffsetX  = 0.0;
    double m_dragOffsetY  = 0.0;
    double m_lastWorldX   = 0.0;
    double m_lastWorldY   = 0.0;
    double m_holdTimer    = 0.0;
    bool   m_dragging     = false;
    bool   m_pendingPress = false;
    static constexpr double DRAG_THRESHOLD = 0.2;

    std::function<void()> m_OnDragStart = {};
    std::function<void()> m_OnDragEnd   = {};
    std::function<void()> m_OnDrag      = {};

public:
    virtual ~Draggable() = default;

    void Init(const Transform& p_transform,
              std::vector<Texture> p_frames,
              std::function<void()> p_onHover      = {},
              std::function<void()> p_onHoverStop  = {},
              std::function<void()> p_onClick      = {},
              std::function<void()> p_onDragStart  = {},
              std::function<void()> p_onDragEnd    = {},
              std::function<void()> p_onDrag       = {})
    {
        Clickable::Init(p_transform, std::move(p_frames),
                        std::move(p_onHover), std::move(p_onHoverStop),
                        std::move(p_onClick));
        m_OnDragStart = std::move(p_onDragStart);
        m_OnDragEnd   = std::move(p_onDragEnd);
        m_OnDrag      = std::move(p_onDrag);
    }

    void Update(double dt) override {
        if (m_pendingPress) {
            m_holdTimer += dt;
            if (m_holdTimer >= DRAG_THRESHOLD) {
                m_dragging     = true;
                m_pendingPress = false;
                if (m_OnDragStart) m_OnDragStart();
            }
        }
    }
    bool OnMouseMoved(const float p_x, const float p_y, const double p_cameraX, const double p_cameraY) override
    {
        m_lastWorldX = p_x + p_cameraX;
        m_lastWorldY = p_y + p_cameraY;
        bool used=Clickable::OnMouseMoved(p_x, p_y, p_cameraX, p_cameraY);
        if (m_dragging) {
            m_transform.x = m_lastWorldX - m_dragOffsetX;
            m_transform.y = m_lastWorldY - m_dragOffsetY;
            if (m_OnDrag) m_OnDrag();
            used = true;
        }
        return used;
    }

    bool OnMousePressed() override {
    if (m_hovered) {
        m_pendingPress = true;
        m_dragging     = false;
        m_holdTimer    = 0.0;
        m_dragOffsetX  = m_lastWorldX - m_transform.x;
        m_dragOffsetY  = m_lastWorldY - m_transform.y;
        return true;
    }
        return false;
}

    bool OnMouseReleased() override {
        bool used=false;
        if (m_dragging) {
            m_dragging = false;
            if (m_OnDragEnd) m_OnDragEnd();
            if (m_hovered) { if (m_OnHover) m_OnHover(); }
            else { if (m_OnHoverStop) m_OnHoverStop(); }
            used = true;
        }
        else if (m_pendingPress && m_hovered) {
            if (m_OnClick) m_OnClick();
            if (m_OnHover) m_OnHover();
            used = true;
        }
        m_pendingPress = false;
        m_holdTimer    = 0.0;
        return used;
    }
};