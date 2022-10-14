#pragma once

#include <vector>

#include "../engine/src/core/math/rbe_math.h"

struct Texture;

namespace ImGuiHelper {

struct TextureRenderTarget {
    Texture* texture = nullptr;
    Rect2 sourceRect { 0.0f, 0.0f, 0.0f, 0.0f };
    Size2D destSize = { 0.0f, 0.0f };
    Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool flipX = false;
    bool flipY = false;
    TransformModel2D* globalTransform = nullptr;
};

class WindowRenderer {
  public:
    void Initialize();
    void Render(const std::vector<TextureRenderTarget>& renderTargets);
    void Finalize();
};
}
