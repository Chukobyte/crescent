#pragma once

#include <string>
#include <vector>

#include "../engine/src/core/math/rbe_math.h"

struct Texture;
struct Font;

namespace ImGuiHelper {
struct TextureRenderTarget {
    Texture *texture = nullptr;
    Rect2 sourceRect{0.0f, 0.0f, 0.0f, 0.0f};
    Size2D destSize = {0.0f, 0.0f};
    Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    bool flipX = false;
    bool flipY = false;
    TransformModel2D *globalTransform = nullptr;
};

struct FontRenderTarget {
    Font* font = nullptr;
    std::string text;
    Vector2 position = { 0.0f, 0.0f };
    float scale = 1.0f;
    Color color = {1.0f, 1.0f, 1.0f, 1.0f};
};

namespace WindowRenderer {
void Render(const std::vector<TextureRenderTarget> &textureRenderTargets, const std::vector<FontRenderTarget>& fontRenderTargets);
} // namespace WindowRenderer
}
