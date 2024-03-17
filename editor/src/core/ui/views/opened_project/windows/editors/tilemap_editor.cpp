#include "tilemap_editor.h"

#include <format>

#include <imgui.h>

#include <seika/rendering/renderer.h>

#include "../engine/src/core/game_properties.h"

#include "../../../../../scene/scene_manager.h"
#include "../../../../../asset_manager.h"

static TilemapComp* cachedComp = nullptr;

void TilemapEditor::Process(SceneNode* node, TilemapComp* tilemapComp) {
    if (!isProcessing) {
        isProcessing = true;
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
        const auto tileSize = cachedComp->GetTileSize();
        const auto mousePos = ImGui::GetMousePos();
        const auto windowPos = ImGui::GetWindowPos();
        const SKAVector2 scroll = { .x = ImGui::GetScrollX(), .y = ImGui::GetScrollY() };
        const auto contentRegionMin = ImGui::GetWindowContentRegionMin();
        const SKAVector2 contentRegionPos = { .x = windowPos.x + contentRegionMin.x, .y = windowPos.y + contentRegionMin.y };
        const SKAVector2 mousePosRelative = {
                .x = mousePos.x - contentRegionPos.x + scroll.x,
                .y = mousePos.y - contentRegionPos.y + scroll.y
        };
        static const SKAVector2 zoom = { .x = 1.0f, .y = 1.0f }; // Assuming zoom is not yet implemented. Adjust if zoom functionality exists.
        SKAVector2i tileCoords = {
                .x = static_cast<int>(mousePosRelative.x / (tileSize.w * zoom.x)),
                .y = static_cast<int>(mousePosRelative.y / (tileSize.h * zoom.y))
        };

        tileCoords.x = ska_math_clamp_int(tileCoords.x, 0, tileSize.w);
        tileCoords.y = ska_math_clamp_int(tileCoords.y, 0, tileSize.h);

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
