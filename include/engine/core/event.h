#pragma once
#include <functional>
#include <type_traits>
enum class EventType
{
    none = 0,
    keyPressed, keyReleased,
    mouseButtonPressed, mouseButtonReleased, mouseMoved, mouseScrolled,windowMinimized,windowRestored,windowWindowed,windowFullscreen
};
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
virtual EventType GetEventType() const override { return GetStaticType(); }

class Event
{
private:
    mutable bool m_handled = false;
public:

    virtual ~Event() = default;
    [[nodiscard]] virtual EventType GetEventType() const = 0;
    [[nodiscard]] bool IsHandled() const { return m_handled; }
    void SetHandled(const bool p_handled)const { m_handled = p_handled; }
};
class EventDispatcher
{
private:
    const Event& m_Event;
public:
    explicit EventDispatcher(const Event& p_event)
        : m_Event(p_event) { }
    template<typename T,typename F>
    bool Dispatch(F&& func)
    {
        static_assert(std::is_base_of_v<Event, T>,
            "T must derive from Event");
        
        static_assert(std::is_invocable_r_v<bool, F, const T&>,
            "F must be a function that is callable with const T & and returns a bool");
        if (m_Event.GetEventType() == T::GetStaticType() && !m_Event.IsHandled())
        {
            m_Event.SetHandled(std::forward<F>(func)(static_cast<const T&>(m_Event)));
            return true;
        }
        return false;
    }
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;
};
