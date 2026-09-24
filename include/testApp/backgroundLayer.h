#pragma once
#include "engine/core/layer.h"
#include "engine/core/event.h"
#include "engine/core/core.h"
#include "engine/basicUiObjects/clickable.h"

class BackgroundLayer : public Layer {
private:
    uint8_t m_r = 25, m_g = 25, m_b = 35, m_a = 255;
    Clickable m_star;

public:
    LAYER_CLASS_TYPE(mainLayer);

    BackgroundLayer() {
        m_star.Init(
            { 0, 0, 1600, 900 },
            {
                { {0, 0, 1600, 1491}, TextureSheets::star }
            }
        );
    }

    ~BackgroundLayer() override = default;

    void OnEvent(const Event &) override {
    }

    void OnUpdate(double p_deltaTime) override {
        m_star.Update(p_deltaTime);
    }

    void OnRender() override {
        m_core->GetWindow()->RenderClear(m_r, m_g, m_b, m_a);
        m_star.Render(m_core->GetWindow(), 0, 0);
    }
};


