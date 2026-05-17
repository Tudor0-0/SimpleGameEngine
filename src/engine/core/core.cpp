#include "engine/core/core.h"
#include <engine/rendering/windowRenderer.h>
#include <functional>
#include <iostream>
#include <ranges>

Core::Core(const WindowSettings& p_windowSettings) {
try {
    m_window = new Window(p_windowSettings);
}
    catch (std::exception &p_exception) {
        std::cerr << p_exception.what() << std::endl;
        return;
    }

}
Core::~Core() {
    if (m_window)delete m_window;
}
void Core::Run() {
    m_running = true;
    BindEventListener(*m_window);
    double lastUpdateTime=m_window->GetTime();
    while (m_running) {
        const double frameStartTime= m_window->GetTime();
        m_window->GetInput();
        const double updateTime= m_window->GetTime();
        const double deltaUpdateTime= updateTime - lastUpdateTime;
        for (const auto & layer : std::views::reverse(m_stackLayers))
            if (layer->IsActive())
                layer->OnUpdate(deltaUpdateTime);
        lastUpdateTime=updateTime;
        FlushLayerCommands();
        m_window->RenderClear();
        for (const std::unique_ptr<Layer> &layer : m_stackLayers)
            if (layer->IsActive())
                layer->OnRender();
        m_window->Render();
        const double frameEndTime= m_window->GetTime();
        double deltaFrameTime= frameEndTime - frameStartTime;
        m_currentFps = static_cast<uint32_t>(1/deltaFrameTime);
      if (deltaFrameTime < m_frameTarget)
            SDL_Delay(static_cast<uint32_t>(1000*(m_frameTarget-deltaFrameTime)));
    }
}
void Core::Stop() {
    m_running = false;
}
void Core::RaiseEvent(const Event &p_event) const {
    for (const auto & layer : std::views::reverse(m_stackLayers))
        if (layer->IsActive())    {
            layer->OnEvent(p_event);
            if (p_event.IsHandled())break;
    }
}
 void Core::BindEventListener(Window &p_window) const {
    p_window.SetEventHandler([this](const Event &p_event) {
        RaiseEvent(p_event);
    });
}
void Core::RegisterLayer(std::unique_ptr<Layer> p_layer) {
    p_layer->SetCore(this);
    m_stackLayers.push_back(std::move(p_layer));
}
void Core::SetFpsTarget(const uint32_t p_targetFps) {
    m_fpsTarget = p_targetFps;
    m_frameTarget = static_cast<double>(1)/p_targetFps;
}
 Window* Core::GetWindow() const {
    return m_window;
}

uint32_t Core::GetCurrentFps() const {
    return m_currentFps;
}
void Core::FlushLayerCommands() {
    for (auto&[commandType, layerType, layer] : m_pendingCommands) {
        if (commandType == LayerCommandType::push) {
            m_stackLayers.push_back(std::move(layer));
            continue ;
        }
        if (commandType == LayerCommandType::focus) {
            for (auto it = m_stackLayers.begin(); it != m_stackLayers.end(); ++it) {
                if ((*it)->GetLayerType() == layerType) {
                    std::rotate(it, it + 1, m_stackLayers.end());
                    break;
                }
            }
            continue;
        }
        if (commandType == LayerCommandType::pop) {
            for (auto it = m_stackLayers.begin(); it != m_stackLayers.end(); ++it) {
                if ((*it)->GetLayerType() == layerType) {
                    m_stackLayers.erase(it);
                    //m_stackLayers.erase(std::next(it).base());
                    break;
                }
            }
        }
    }
    m_pendingCommands.clear();
}