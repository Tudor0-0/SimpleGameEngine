#pragma once
#include "SDL2/SDL.h"
inline SDL_FRect Offset(SDL_FRect p_rect, const double &p_x, const double &p_y) {
    p_rect.x = static_cast<float>(p_x+p_rect.x);
    p_rect.y = static_cast<float>(p_y+p_rect.y);
    return p_rect;
}

inline SDL_FRect Offset(SDL_Rect p_rect, const double &p_x, const double &p_y) {
    SDL_FRect outRect;
    outRect.x = static_cast<float>(p_x+p_rect.x);
    outRect.y = static_cast<float>(p_y+p_rect.y);
    outRect.w = static_cast<float>(p_rect.w);
    outRect.h = static_cast<float>(p_rect.h);
    return outRect;
}
/*
inline SDL_FRect Scale(SDL_FRect p_rect, const float &p_scale=1) {
    p_rect.w=static_cast<float>(p_scale*p_rect.w);
    p_rect.h=static_cast<float>(p_scale*p_rect.h);
    return p_rect;
}*/
inline SDL_FRect Scale(const SDL_FRect& p_rect, const SDL_FPoint& p_pivot, float p_scale=1) {
    SDL_FRect result;
    result.w = static_cast<float>(p_rect.w * p_scale);
    result.h = static_cast<float>(p_rect.h * p_scale);
    result.x = static_cast<float>(p_pivot.x + (p_rect.x - p_pivot.x) * p_scale);
    result.y = static_cast<float>(p_pivot.y + (p_rect.y - p_pivot.y) * p_scale);
    return result;
}
inline SDL_FRect Scale(const SDL_Rect& p_rect, const SDL_FPoint& p_pivot, float p_scale=1) {
    SDL_FRect result;
    result.w = static_cast<float>(p_rect.w)* p_scale;
    result.h = static_cast<float>(p_rect.h) * p_scale;
    result.x = static_cast<float>(p_pivot.x) + (static_cast<float>(p_rect.x) - p_pivot.x) * p_scale;
    result.y = static_cast<float>(p_pivot.y) + (static_cast<float>(p_rect.y) - p_pivot.y) * p_scale;
    return result;
}

inline SDL_FRect Scale(const SDL_FRect& p_rect, float p_scale=1.0f) {
    const SDL_FPoint center = {
        p_rect.x + (p_rect.w / 2.0f),
        p_rect.y + (p_rect.h / 2.0f)
    };
    return Scale(p_rect, center, p_scale);
}

inline SDL_FRect Scale(const SDL_Rect& p_rect, float p_scale=1.0f) {
    const SDL_FPoint center = {
        static_cast<float>(p_rect.x) + (static_cast<float>(p_rect.w) / 2.0f),
        static_cast<float>(p_rect.y) + (static_cast<float>(p_rect.h) / 2.0f)
    };
    return Scale(p_rect, center, p_scale);
}
