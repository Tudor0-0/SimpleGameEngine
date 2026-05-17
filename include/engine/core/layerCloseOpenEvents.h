#pragma once
#include <memory>
#include "engine/core/layer.h"
enum class LayerCommandType { push, pop ,focus};
struct LayerCommand {
    LayerCommandType commandType;
    LayerType layerType;
    std::unique_ptr<Layer> layer;
};
