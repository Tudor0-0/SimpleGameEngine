#pragma once
#include <utility>
#include "engine/rendering/windowRenderer.h"
class Core;
enum class LayerType {
    none = 0,
    mainLayer,debugLayer,menuLayer,testLayer,overlayLayer,
};
#define LAYER_CLASS_TYPE(type) static LayerType GetStaticType() { return LayerType::type; }\
virtual LayerType GetLayerType() const override { return GetStaticType(); }

class Layer {
protected:
    Core* m_core = nullptr;
    std::string m_debugName;
    bool m_isActive=true;

public:
    explicit Layer(std::string  p_name = "Layer") : m_debugName(std::move(p_name)) {}
    virtual ~Layer() = default;

    virtual void OnUpdate(double p_deltaTime)   = 0;
    virtual void OnRender()  = 0;
    virtual void OnEvent(const Event& p_event) = 0;

    [[nodiscard]] virtual LayerType GetLayerType() const = 0;

    void SetCore(Core* p_core);
    [[nodiscard]] bool IsActive() const;
    void SetActive(bool p_active);
    [[nodiscard]] inline const std::string& GetName() const { return m_debugName; }

};