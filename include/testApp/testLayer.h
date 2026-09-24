    #pragma once
#include <iostream>
#include <vector>
#include "engine/core/layer.h"
#include "engine/core/event.h"
#include "engine/core/inputEvents.h"
#include "engine/core/core.h"
#include "engine/basicUiObjects/clickable.h"
#include "engine/basicUiObjects/draggable.h"

class TestLayer: public Layer {
private:
    uint8_t r=255,g=0,b=0,a=50,cnt=0,fullscreen=0;
    bool changing=false;
    std::vector<std::shared_ptr<Draggable>> m_buttons;
    double m_cameraX = 0, m_cameraY = 0;
public:
    void OnEvent(const Event &p_event) override {
        EventDispatcher dispatcher(p_event);
        dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent &event) -> bool {
            if (event.GetKeyCode() == SDL_SCANCODE_ESCAPE) {
                m_core->Stop();
                return true;
            }
            if (event.GetKeyCode() == SDL_SCANCODE_C) {
                changing=true;
               return true;
           }
            if (event.GetKeyCode() == SDL_SCANCODE_N) {
               m_core->PushLayer(std::make_unique<TestLayer>());
                std::cout<<"N"<<'\n';
                return true;
            }
            if (event.GetKeyCode() == SDL_SCANCODE_W) {
              m_cameraY-=10;
               return true;
           }
            if (event.GetKeyCode() == SDL_SCANCODE_S) {
              m_cameraY+=10;
               return true;
           }
            if (event.GetKeyCode() == SDL_SCANCODE_A) {
            m_cameraX-=10;
             return true;
         }
          if (event.GetKeyCode() == SDL_SCANCODE_D) {
            m_cameraX+=10;
             return true;
         }
            if (event.GetKeyCode() == SDL_SCANCODE_R) {
                if (fullscreen == 0) {
                    m_core->GetWindow()->SetResolutionAndScaling(1920, 1080, 1600, 900);
                    fullscreen = 1;
                } else {
                    m_core->GetWindow()->SetResolutionAndScaling(1600, 900, 1600, 900);
                    fullscreen = 0;
                }
                return true;
            }
            return false;
        });
        dispatcher.Dispatch<KeyReleasedEvent>([this](const KeyReleasedEvent &event) -> bool {
            if (event.GetKeyCode() == SDL_SCANCODE_C) {
                changing=false;
                return true;
            }
            return false;
        });
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &event) -> bool {
            if (event.GetButtonCode() == SDL_BUTTON_LEFT) {
                for (auto it=m_buttons.begin();it!=m_buttons.end();++it) if ((*it)->OnMousePressed()){
                    std::rotate(m_buttons.begin(),it,it+1);
                    return true;
                }
                return false;
            }
            if (event.GetButtonCode() == SDL_BUTTON_RIGHT) {
                r=100,g=20,b=150,a=155;
                return true;
            }
            return false;
        });
        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent &event) -> bool {
            if (event.GetButtonCode() == SDL_BUTTON_LEFT) {
                for (auto it=m_buttons.begin();it!=m_buttons.end();++it) if ((*it)->OnMouseReleased()){
                    std::rotate(m_buttons.begin(),it,it+1);
                    return true;
                }
                return false;
            }
            return false;
        });
        dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent &event) -> bool{
            const float posX = event.GetXPos();
            const float posY=event.GetYPos();
            for (auto it=m_buttons.begin();it!=m_buttons.end();++it) if ((*it)->OnMouseMoved(posX,posY,m_cameraX,m_cameraY)){
                     std::rotate(m_buttons.begin(),it,it+1);
                     return true;
                 }
            return false;
        });
    }
    void OnUpdate(double p_deltaTime) override {
        cnt++;
        for (auto it=m_buttons.begin();it!=m_buttons.end();++it) {
            (*it)->Update(p_deltaTime);
        }
        if (changing) {
            r=rand()%256;
            g=rand()%256;
            b=rand()%256;
            a=rand()%256;
        }
    }
    void OnRender() override {
        m_core->GetWindow()->RenderClear(r,g,b,a);
        for (auto it=m_buttons.rbegin();it!=m_buttons.rend();++it) {
            (*it)->Render(m_core->GetWindow(),m_cameraX,m_cameraY);
        }
    }
    LAYER_CLASS_TYPE(testLayer);
    ~TestLayer() override = default;
    TestLayer() {
        int n = 3;

        for (int i = 0; i < n; ++i) {
            auto currentBtn = std::make_shared<Draggable>();
            Draggable* rawBtn = currentBtn.get();

            float posX = 200.0f;
            float posY = 200.0f + (i * 32 * 6);

            rawBtn->Init(
                {posX, posY, 48*5, 32*5},
                {
                    {{0,   0, 48, 32}, TextureSheets::testButtons},
                    {{49,  0, 48, 32}, TextureSheets::testButtons},
                    {{97,  0, 48, 32}, TextureSheets::testButtons},
                    {{144, 0, 48, 32}, TextureSheets::testButtons},
                },
                [this,rawBtn]() { rawBtn->SetCurrentFrame(1); rawBtn->SetScale(1); },
                [this,rawBtn]() { rawBtn->SetCurrentFrame(0); },
                [this, rawBtn]() {
                    rawBtn->SetCurrentFrame(2);
                    rawBtn->SetScale(0.90);
                    r = rand() % 256;
                    g = rand() % 256;
                    b = rand() % 256;
                    a = rand() % 256;
                },
                [this,rawBtn]() { rawBtn->SetCurrentFrame(2); rawBtn->SetScale(0.90); },
                [this,rawBtn]() { rawBtn->SetCurrentFrame(1); rawBtn->SetScale(1); },
                []() {}
            );

            m_buttons.push_back(currentBtn);
        }
    }
};



