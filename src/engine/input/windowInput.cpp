#pragma once
#include "engine/rendering/windowRenderer.h"
#include "engine/utils/exceptions.h"
#include "engine/core/event.h"
#include "engine/core/inputEvents.h"
#include <cstring>

void Window::SetEventHandler(std::function<void(const Event &)> p_eventSender) {
    m_eventSender = std::move(p_eventSender);
}

void Window::GetInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                m_eventSender(WindowMinimizedEvent());
            if (event.window.event == SDL_WINDOWEVENT_RESTORED)
                m_eventSender(WindowRestoredEvent());
        }
    const uint8_t *newKeyState = SDL_GetKeyboardState(nullptr);
    uint8_t *oldKeyState = m_keyState;
    for (uint32_t i = 0; i < SDL_NUM_SCANCODES; i++)
        if (newKeyState[i] != oldKeyState[i]) {
            if (newKeyState[i])m_eventSender(KeyPressedEvent(i));
            else m_eventSender(KeyReleasedEvent(i));
        }
    memcpy(oldKeyState, newKeyState, SDL_NUM_SCANCODES);
    int32_t mouseX, mouseY;
    const uint32_t newMouseState = SDL_GetMouseState(&mouseX, &mouseY);
    float dmouseX , dmouseY;
    SDL_RenderWindowToLogical(m_renderer, mouseX,mouseY, &dmouseX, &dmouseY);
    if (dmouseX != m_mouseX || dmouseY != m_mouseY)
        m_eventSender(MouseMovedEvent(dmouseX, dmouseY));
    for (uint32_t i = 1; i <= 5; i++)
        if ((newMouseState & SDL_BUTTON(i)) != (m_mouseState & SDL_BUTTON(i))) {
            if (newMouseState & SDL_BUTTON(i))m_eventSender(MouseButtonPressedEvent(i));
            else m_eventSender(MouseButtonReleasedEvent(i));
        }
    m_mouseX = dmouseX;
    m_mouseY = dmouseY;
    m_mouseState = newMouseState;
}