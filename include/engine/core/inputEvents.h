#pragma once
#include <cstdint>
#include "engine/core/event.h"
class KeyEvent: public Event {
protected:
     uint32_t m_keyCode;
public:
     explicit KeyEvent(const uint32_t &p_keyCode) : m_keyCode(p_keyCode) {}
     uint32_t GetKeyCode() const { return m_keyCode; }
};
class KeyPressedEvent: public KeyEvent {
public:
     explicit KeyPressedEvent(const uint32_t &p_keyCode) : KeyEvent(p_keyCode) {}
     EVENT_CLASS_TYPE(keyPressed)
};
class KeyReleasedEvent: public KeyEvent {
public:
     explicit KeyReleasedEvent(const uint32_t &p_keyCode) : KeyEvent(p_keyCode) {}
     EVENT_CLASS_TYPE(keyReleased)
};
class MouseMovedEvent: public Event {
private:
     float m_xPos,m_yPos;
public:
     MouseMovedEvent(const float& p_xPos,const float& p_yPos) : m_xPos(p_xPos),m_yPos(p_yPos) {}
     EVENT_CLASS_TYPE(mouseMoved)
     inline float GetXPos() const { return m_xPos; }
     inline float GetYPos() const { return m_yPos; }
};
class MouseButtonEvent: public Event {
private:
     uint32_t m_buttonCode;
public:
     explicit MouseButtonEvent(const uint32_t &p_button) : m_buttonCode(p_button) {}
     uint32_t GetButtonCode() const { return m_buttonCode; }
};
class MouseButtonPressedEvent: public MouseButtonEvent {
public:
     explicit MouseButtonPressedEvent(const uint32_t &p_button) : MouseButtonEvent(p_button) {}
     EVENT_CLASS_TYPE(mouseButtonPressed)
};
class MouseButtonReleasedEvent: public MouseButtonEvent {
public:
     explicit MouseButtonReleasedEvent(const uint32_t &p_button) : MouseButtonEvent(p_button) {}
     EVENT_CLASS_TYPE(mouseButtonReleased)
};
class WindowMinimizedEvent: public Event {
public:
     EVENT_CLASS_TYPE(windowMinimized)
};
class WindowRestoredEvent: public Event {
public:
     EVENT_CLASS_TYPE(windowRestored)
};
class WindowWindowedEvent: public Event {
public:
     EVENT_CLASS_TYPE(windowWindowed)
};
class WindowFullscreenEvent: public Event {
public:
     EVENT_CLASS_TYPE(windowFullscreen)
};