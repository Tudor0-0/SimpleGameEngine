#include "engine/core/layer.h"
#include "engine/core/core.h"

bool Layer::IsActive() const {
    return m_isActive;
}
void Layer::SetActive(const bool p_active) {
    m_isActive = p_active;
}
void Layer::SetCore(Core* p_core) {
    m_core = p_core;
}
