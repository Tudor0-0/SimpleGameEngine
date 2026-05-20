/*#pragma once
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
    Clickable Steluta;
public:
    void OnEvent(const Event &p_event) override {
        EventDispatcher dispatcher(p_event);
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &event) -> bool {
            if (event.GetButtonCode() == SDL_BUTTON_LEFT) {
                for (auto it=butoane.begin();it!=butoane.end();++it) if ((*it)->OnMousePressed()){
                    std::rotate(butoane.begin(),it,it+1);
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
                for (auto it=butoane.begin();it!=butoane.end();++it) if ((*it)->OnMouseReleased()){
                    std::rotate(butoane.begin(),it,it+1);
                    return true;
                }
                return false;
            }
            return false;
        });
        dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent &event) -> bool{
            const float posX = event.GetXPos();
            const float posY=event.GetYPos();
            for (auto it=butoane.begin();it!=butoane.end();++it) if ((*it)->OnMouseMoved(posX,posY,camerax,cameray)){
                     std::rotate(butoane.begin(),it,it+1);
                     return true;
                 }
            return false;
        });
    }
    void OnUpdate(double p_deltaTime) override {
        cnt++;
        if (changing) {
            r=rand()%256;
            g=rand()%256;
            b=rand()%256;
            a=rand()%256;
        }
    }
    void OnRender() override {

        if (cnt==255) {
           cnt=0;
        }
    }
    LAYER_CLASS_TYPE(testLayer);
    ~TestLayer() {
        std::cout<<"~testLayer()"<<'\n';
    }
    TestLayer() {
       Steluta.init({TextureSheets::star},);
    }
};
*/

