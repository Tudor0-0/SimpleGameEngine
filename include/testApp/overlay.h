#pragma once
#include <iostream>

#include "engine/core/layer.h"
#include "engine/core/event.h"
#include "engine/core/inputEvents.h"
#include "engine/core/core.h"
#include "engine/basicUiObjects/clickable.h"
#include "engine/basicUiObjects/draggable.h"
#define FPS
#define TEXTURE_BUTTON(buttonName,buttonPoz,buttonState)Texture buttonName={{48*buttonState,32*buttonPoz,48,32},TextureSheets::wControlButtons}
class Overlay : public Layer {
private:
    bool fullscreen=false;
    //bool minimized=false;

    TEXTURE_BUTTON(closeNormal,0,0);
    TEXTURE_BUTTON(closeHovered,0,1);
    TEXTURE_BUTTON(closePressed,0,2);
    TEXTURE_BUTTON(minimizeNormal,1,0);
    TEXTURE_BUTTON(minimizeHovered,1,1);
    TEXTURE_BUTTON(minimizePressed,1,2);
    TEXTURE_BUTTON(restoreNormal,2,0);
    TEXTURE_BUTTON(restoreHovered,2,1);
    TEXTURE_BUTTON(restorePressed,2,2);

    Clickable m_close,m_minimize,m_restore;

    //double camerax=0,cameray=0;
public:
    LAYER_CLASS_TYPE(overlayLayer);
    void OnEvent(const Event &p_event) override {
        EventDispatcher dispatcher(p_event);
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &event) -> bool {
            if (event.GetButtonCode() == SDL_BUTTON_LEFT) {
                if (m_close.OnMousePressed())return true;
                if (m_minimize.OnMousePressed())return true;
                if (m_restore.OnMousePressed())return true;
            }
            return false;
        });
        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent &event) -> bool {
            if (event.GetButtonCode() == SDL_BUTTON_LEFT) {
                if (m_close.OnMouseReleased())return true;
                if (m_minimize.OnMouseReleased())return true;
                if (m_restore.OnMouseReleased())return true;
            }
            return false;
        });
        dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent &event) -> bool {
            m_close.OnMouseMoved(event.GetXPos(),event.GetYPos(),0,0);
            m_restore.OnMouseMoved(event.GetXPos(),event.GetYPos(),0,0);
            m_minimize.OnMouseMoved(event.GetXPos(),event.GetYPos(),0,0);
           return false;
        });
    }
    void OnUpdate(double ) override {
    }
    void OnRender() override{
#ifdef FPS
    #ifdef _WIN32
         system("cls");

    #elif defined(__APPLE__) || defined(__linux__)
        printf("\033[2J\033[H");
    #endif
        std::cout<<m_core->GetCurrentFps()<<'\n';
        std::cout<<std::flush;
#endif
        m_core->GetWindow()->RenderRect();
        m_close.Render(m_core->GetWindow(),0,0);
        m_minimize.Render(m_core->GetWindow(),0,0);
        m_restore.Render(m_core->GetWindow(),0,0);
    }

    ~Overlay() override{
        std::cout<<"~overlayLayer()"<<'\n';
    }
    Overlay() {
        m_close.Init({ 1600-48, 0 , 48 , 32 },
            {closeNormal,closeHovered,closePressed},
            [this](){m_close.SetCurrentFrame(1);},
            [this](){m_close.SetCurrentFrame(0);},
            [this](){m_close.SetCurrentFrame(2);},
            [this](){m_close.SetCurrentFrame(1);m_core->Stop();},
            [this](){m_close.SetCurrentFrame(1);}
        );
        m_restore.Init({ 1600-48*2, 0 , 48 , 32 },
            {restoreNormal,restoreHovered,restorePressed},
            [this](){m_restore.SetCurrentFrame(1);},
            [this](){m_restore.SetCurrentFrame(0);},
            [this](){m_restore.SetCurrentFrame(2);},
            [this]() {
               m_restore.SetCurrentFrame(1);
                if (fullscreen==true)
                 m_core->GetWindow()->SetResolutionAndScaling(1600,900,1600,900),fullscreen=false;
                else
                    m_core->GetWindow()->SetResolutionAndScaling(1920,1080,1600,900),fullscreen=true;
            },
            [this](){m_restore.SetCurrentFrame(1);}
        );
        m_minimize.Init({ 1600-48*3, 0 , 48 , 32 },
           {minimizeNormal,minimizeHovered,minimizePressed},
           [this](){ m_minimize.SetCurrentFrame(1);},
           [this](){ m_minimize.SetCurrentFrame(0);},
           [this](){ m_minimize.SetCurrentFrame(2);},
           [this]() {
                m_minimize.SetCurrentFrame(1);
               m_core->GetWindow()->Minimize();
              },
              [this](){ m_minimize.SetCurrentFrame(1);}
          );
    }
};