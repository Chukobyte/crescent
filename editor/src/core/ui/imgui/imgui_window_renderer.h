#pragma once

#include <string>
#include <vector>

#include <seika/math/math.h>

struct SkaTexture;
struct SkaFont;

namespace ImGuiHelper {
struct TextureRenderTarget {
    SkaTexture* texture = nullptr;
    SkaRect2 sourceRect = SKA_RECT2D_ZERO;
    SkaSize2D destSize = SKA_SIZE2D_ZERO;
    SkaColor color = SKA_COLOR_WHITE;
    bool flipH = false;
    bool flipV = false;
    int zIndex = 0;
    bool useGlobalTransform = true;
    union {
        SkaTransformModel2D* globalTransform = nullptr;
        SkaTransform2D transform2D;
    };
};

struct FontRenderTarget {
    SkaFont* font = nullptr;
    std::string text;
    SkaVector2 position = SKA_VECTOR2_ZERO;
    float scale = 1.0f;
    SkaColor color = SKA_COLOR_WHITE;
    int zIndex = 0;
};

namespace WindowRenderer {
void Render(const std::vector<TextureRenderTarget> &textureRenderTargets, const std::vector<FontRenderTarget>& fontRenderTargets);
} // namespace WindowRenderer
}
