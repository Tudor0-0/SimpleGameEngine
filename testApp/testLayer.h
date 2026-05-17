#pragma once
#include <iostream>

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
    Texture normal={{0,0,48,32},TextureSheets::testButtons};
    Texture hovered={{49,0,48,32},TextureSheets::testButtons};
    Texture pressed={{97,0,48,32},TextureSheets::testButtons};
    Texture clicked={{144,0,48,32},TextureSheets::testButtons};
    Draggable button1;
    Draggable button2;
    double camerax=0,cameray=0;
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
              cameray-=10;
               return true;
           }
            if (event.GetKeyCode() == SDL_SCANCODE_S) {
              cameray+=10;
               return true;
           }
            if (event.GetKeyCode() == SDL_SCANCODE_A) {
            camerax-=10;
             return true;
         }
          if (event.GetKeyCode() == SDL_SCANCODE_D) {
            camerax+=10;
             return true;
         }
            if (event.GetKeyCode() == SDL_SCANCODE_R) {
                if (fullscreen==0)
            m_core->GetWindow()->SetResolutionAndScaling(1920,1080,1920,1080),fullscreen=1;
                else
                    m_core->GetWindow()->SetResolutionAndScaling(1600,900,1920,1080),fullscreen=0;

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
                button1.OnMousePressed();
                button2.OnMousePressed();
                return true;
            }
            if (event.GetButtonCode() == SDL_BUTTON_RIGHT) {
                r=100,g=20,b=150,a=155;
                return true;
            }
            return false;
        });
        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent &event) -> bool {
            if (event.GetButtonCode() == SDL_BUTTON_LEFT) {
                button1.OnMouseReleased();
                button2.OnMouseReleased();
                return true;
            }
            return false;
        });
        dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent &event) -> bool {
            button1.OnMouseMoved(event.GetXPos(), event.GetYPos(),camerax,cameray);
            button2.OnMouseMoved(event.GetXPos(), event.GetYPos(),camerax,cameray);
           return false;
        });
    }
    void OnUpdate(double p_deltaTime) override {
        cnt++;
        button1.Update(p_deltaTime);
        button2.Update(p_deltaTime);
        if (changing) {
            r=rand()%256;
            g=rand()%256;
            b=rand()%256;
            a=rand()%256;
        }
    }
    void OnRender() override {
        m_core->GetWindow()->RenderClear(r,g,b,a);
        button1.Render(m_core->GetWindow(),camerax,cameray);
        button2.Render(m_core->GetWindow(),camerax,cameray);
        if (cnt==255) {
           //std::cout<<"here"<<'\n';;
        }
    }
    LAYER_CLASS_TYPE(testLayer);
    ~TestLayer() {
        std::cout<<"~testLayer()"<<'\n';
    }
    TestLayer() {
        button1.Init(
     {200, 200, 48*5, 32*5},
     {
         {{0,   0, 48, 32}, TextureSheets::testButtons},
         {{49,  0, 48, 32}, TextureSheets::testButtons},
         {{97,  0, 48, 32}, TextureSheets::testButtons},
         {{144, 0, 48, 32}, TextureSheets::testButtons},
     },
     [this]() { button1.SetCurrentFrame(1); },
     [this]() { button1.SetCurrentFrame(0); },
     [this]() {
         button1.SetCurrentFrame(2);
         r = rand() % 256;
         g = rand() % 256;
         b = rand() % 256;
         a = rand() % 256;
     },
     [this]() { button1.SetCurrentFrame(2); },
     [this]() { button1.SetCurrentFrame(1); },
     [this]() {}
 );
        button2.Init(
    {200, 200-32*6, 48*5, 32*5},
    {
        {{0,   0, 48, 32}, TextureSheets::testButtons},
        {{49,  0, 48, 32}, TextureSheets::testButtons},
        {{97,  0, 48, 32}, TextureSheets::testButtons},
        {{144, 0, 48, 32}, TextureSheets::testButtons},
    },
    [this]() { button2.SetCurrentFrame(1); button2.SetScale(1); }, // onHover,              // onHover
    [this]() { button2.SetCurrentFrame(0); },              // onHoverStop
    [this]() {                                             // onClick (quick click)
        button2.SetCurrentFrame(2);
        button2.SetScale(0.90);
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;
        a = rand() % 256;
    },
    [this]() { button2.SetCurrentFrame(2); button2.SetScale(0.90); }, // onDragStart
    [this]() { button2.SetCurrentFrame(1); button2.SetScale(1); },    // onDragEnd
    [this]() {}                                                        // onDrag
);

    }
};



