#include "tilemap_editor.h"

#include <format>

#include <imgui.h>

#include <seika/rendering/renderer.h>

#include "../engine/src/core/game_properties.h"

#include "../../../../../scene/scene_manager.h"
#include "../../../../../asset_manager.h"
#include "../../../../../project_properties.h"


static TilemapComp* cachedComp = nullptr;
SETexture* colorRectTexture = nullptr;

void TilemapEditor::Process(SceneNode* node, TilemapComp* tilemapComp) {
    if (!isProcessing) {
        isProcessing = true;
        if (!colorRectTexture) {
            colorRectTexture = se_texture_create_solid_colored_texture(1, 1, 255);
        }
    }
    selectedNodeUID = node->GetUID();
    cachedComp = tilemapComp;
}

void TilemapEditor::End() {
    if (isProcessing) {
        isProcessing = false;
        selectedNodeUID.reset();
    }
}

bool TilemapEditor::IsNodeSelected(SceneNode* node) {
    return selectedNodeUID.has_value() && selectedNodeUID.value() == node->GetUID();
}

std::vector<ImGuiHelper::FontRenderTarget> TilemapEditor::GetFontRenderTargets() const {
    if (isProcessing) {
        const auto tileCoords = GetMouseTileCoords();
        const std::string formattedText = std::format("Tile Coords: ({}, {})", tileCoords.x, tileCoords.y);

        return {
                {
                    .font = AssetManager::Get()->GetFont(CRE_DEFAULT_FONT_ASSET.uid),
                    .text = formattedText,
                    .position = SKAVector2{ 16.0f, 16.0f },
                    .scale = 1.0f,
                    .color = SKA_COLOR_WHITE,
                    .zIndex = SE_RENDERER_MAX_Z_INDEX
                }
        };
    }
    return {};
}

std::vector<ImGuiHelper::TextureRenderTarget> TilemapEditor::GetTextureRenderTargets() const {
    if (isProcessing) {
        const auto tileCoords = GetMouseTileCoords();
        const auto tileSize = cachedComp->GetTileSize();
        return {
                {
                    .texture = colorRectTexture,
                    .sourceRect = { .x = 0.0f, .y = 0.0f, .w = 1.0f, .h = 1.0f },
                    .destSize = { .w = (float)tileSize.w, .h = (float)tileSize.h },
                    .color = SKA_COLOR_WHITE,
                    .flipH = false,
                    .flipV = false,
                    .zIndex = SE_RENDERER_MAX_Z_INDEX,
                    .useGlobalTransform = false,
                    .transform2D = {
                        .position = { .x = static_cast<float>(tileCoords.x * tileSize.w), .y = static_cast<float>(tileCoords.y * tileSize.h) },
                        .scale = SKA_VECTOR2_ONE,
                        .rotation = 0.0f
                    }
                }
        };
    }
    return {};
}

SKAVector2i TilemapEditor::GetMouseTileCoords() {
    const auto* gameProperties = ProjectProperties::Get();
    const auto tileSize = cachedComp->GetTileSize();
    const auto mousePos = ImGui::GetMousePos();
    const auto windowPos = ImGui::GetWindowPos();
    const auto windowSize = ImGui::GetWindowSize();
    const SKAVector2 scroll = { .x = ImGui::GetScrollX(), .y = ImGui::GetScrollY() };
    const SKAVector2 zoom = {
            .x = windowSize.x / static_cast<float>(gameProperties->resolutionWidth),
            .y = windowSize.y / static_cast<float>(gameProperties->resolutionHeight)
    };
    const SKAVector2 mousePosRelative = {
            .x = mousePos.x - windowPos.x + scroll.x,
            .y = mousePos.y - windowPos.y + scroll.y
    };
    SKAVector2i tileCoords = {
            .x = static_cast<int>(mousePosRelative.x / ((float)tileSize.w * zoom.x)),
            .y = static_cast<int>(mousePosRelative.y / ((float)tileSize.h * zoom.y))
    };
    tileCoords.x = ska_math_clamp_int(tileCoords.x, 0, tileSize.w);
    tileCoords.y = ska_math_clamp_int(tileCoords.y, 0, tileSize.h);
    return tileCoords;
}
