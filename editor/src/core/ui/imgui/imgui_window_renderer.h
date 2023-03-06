#pragma once

#include <string>
#include <vector>

#include "../seika/src/math/se_math.h"

struct SETexture;
struct SEFont;

namespace ImGuiHelper {
struct TextureRenderTarget {
    SETexture *texture = nullptr;
    SERect2 sourceRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    SESize2D destSize = { 0.0f, 0.0f };
    SEColor color = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool flipH = false;
    bool flipV = false;
    SETransformModel2D *globalTransform = nullptr;
    int zIndex = 0;
};

struct FontRenderTarget {
    SEFont* font = nullptr;
    std::string text;
    SEVector2 position = { 0.0f, 0.0f };
    float scale = 1.0f;
    SEColor color = { 1.0f, 1.0f, 1.0f, 1.0f };
    int zIndex = 0;
};

namespace WindowRenderer {
void Render(const std::vector<TextureRenderTarget> &textureRenderTargets, const std::vector<FontRenderTarget>& fontRenderTargets);
} // namespace WindowRenderer
}
