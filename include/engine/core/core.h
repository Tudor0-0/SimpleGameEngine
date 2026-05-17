#pragma once
#include <memory>
#include <vector>

#include "event.h"
#include "engine/core/layer.h"
#include "engine/rendering/windowRenderer.h"
#include "engine/core/layerCloseOpenEvents.h"


class Core {
private:
    Window *m_window=nullptr;
    std::vector<std::unique_ptr<Layer>> m_stackLayers;
    std::vector<LayerCommand> m_pendingCommands;
    bool m_running=false;
    uint32_t m_fpsTarget = 999999 , m_currentFps=0;
    double m_frameTarget = 1.0/m_fpsTarget; //in secunde
public:
    explicit Core(const WindowSettings& p_windowSettings=WindowSettings());
    virtual ~Core();
    void Run();
    void Stop();
    void RegisterLayer(std::unique_ptr<Layer> p_layer);

    void SetFpsTarget(uint32_t p_targetFps);

    [[nodiscard]] Window *GetWindow() const;

    template<typename T>
    T* GetLayer() {
        for (const auto& layer : m_stackLayers) {
            if (T::GetStaticType() == layer->GetLayerType()) {
                return static_cast<T*>(layer.get());
            }
        }
        return nullptr;
    }
    /*template<typename T>
   std::vector<std::unique_ptr<Layer>>::iterator
    GetLayerIterator() {
        for (auto it=m_stackLayers.begin(); it != m_stackLayers.end(); ++it) {
            if (T::GetStaticType() == (*it)->GetLayerType()) {
                return it;
            }
        }
        return m_stackLayers.end();
    }*/
    //aparent exista std::rotate
    void EraseLayer(Layer *layer) {
        if (!layer)return;
        layer->SetActive(false);
        LayerCommand command;
        command.layerType=layer->GetLayerType();
        command.commandType=LayerCommandType::pop;
        m_pendingCommands.push_back(std::move(command));
    }
    template<typename T>
    void EraseLayer() {
        EraseLayer(GetLayer<T>());
    }

    template<typename T>
    void FocusLayer() {
        LayerCommand command;
        command.layerType=T::GetStaticType();
        command.commandType=LayerCommandType::focus;
        m_pendingCommands.push_back(std::move(command));
    }
    void FocusLayer(const Layer *layer) {
        if (!layer)return;
        LayerCommand command;
        command.layerType=layer->GetLayerType();
        command.commandType=LayerCommandType::focus;
        m_pendingCommands.push_back(std::move(command));
    }
    void PushLayer(std::unique_ptr<Layer> p_layer) {
        p_layer->SetCore(this);
        LayerCommand command;
        command.layer=std::move(p_layer);
        command.commandType=LayerCommandType::push;
        m_pendingCommands.push_back(std::move(command));
     }
    void RaiseEvent(const Event &p_event) const;
    void BindEventListener(Window &p_window) const;
    [[nodiscard]] uint32_t GetCurrentFps() const;

    void FlushLayerCommands();
};
