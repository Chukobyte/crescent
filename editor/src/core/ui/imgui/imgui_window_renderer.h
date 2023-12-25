#pragma once

#include <string>
#include <vector>

#include <seika/math/se_math.h>

struct SETexture;
struct SEFont;

namespace ImGuiHelper {
struct TextureRenderTarget {
    SETexture *texture = nullptr;
    SKARect2 sourceRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    SKASize2D destSize = { 0.0f, 0.0f };
    SKAColor color = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool flipH = false;
    bool flipV = false;
    SKATransformModel2D *globalTransform = nullptr;
    int zIndex = 0;
};

struct FontRenderTarget {
    SEFont* font = nullptr;
    std::string text;
    SKAVector2 position = { 0.0f, 0.0f };
    float scale = 1.0f;
    SKAColor color = { 1.0f, 1.0f, 1.0f, 1.0f };
    int zIndex = 0;
};

namespace WindowRenderer {
void Render(const std::vector<TextureRenderTarget> &textureRenderTargets, const std::vector<FontRenderTarget>& fontRenderTargets);
} // namespace WindowRenderer
}
