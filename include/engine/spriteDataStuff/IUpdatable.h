#pragma once
#include "SDL.h"

//TODO nush cum sa numesc acest header
class IUpdatable {
public:
    virtual void Update(double dt) = 0;
    virtual ~IUpdatable() = default;
};
struct Transform {
    double x=0, y=0;
    float width=0, height=0;
    float angle = 0.0;
    SDL_FPoint p_center = {0,0};
    SDL_RendererFlip p_flip = SDL_FLIP_NONE;
    float scale = 1.0;
};