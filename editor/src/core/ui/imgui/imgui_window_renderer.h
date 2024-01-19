#pragma once

#include <string>
#include <vector>

#include <seika/math/se_math.h>

struct SETexture;
struct SEFont;

namespace ImGuiHelper {
struct TextureRenderTarget {
    SETexture *texture = nullptr;
    SKARect2 sourceRect = SKA_RECT2D_ZERO;
    SKASize2D destSize = SKA_SIZE2D_ZERO;
    SKAColor color = SKA_COLOR_WHITE;
    bool flipH = false;
    bool flipV = false;
    SKATransformModel2D* globalTransform = nullptr;
    int zIndex = 0;
};

struct FontRenderTarget {
    SEFont* font = nullptr;
    std::string text;
    SKAVector2 position = SKA_VECTOR2_ZERO;
    float scale = 1.0f;
    SKAColor color = SKA_COLOR_WHITE;
    int zIndex = 0;
};

namespace WindowRenderer {
void Render(const std::vector<TextureRenderTarget> &textureRenderTargets, const std::vector<FontRenderTarget>& fontRenderTargets);
} // namespace WindowRenderer
}
