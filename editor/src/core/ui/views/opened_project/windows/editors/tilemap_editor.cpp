#include "tilemap_editor.h"

#include <imgui.h>

#include "../../../../../scene/scene_manager.h"

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

std::vector<ImGuiHelper::TextureRenderTarget> TilemapEditor::GetRenderTargets() const {
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

        ImGui::Text("Tile Coords: (%d, %d)", tileCoords.x, tileCoords.y);
    }
    return {};
}

bool TilemapEditor::IsNodeSelected(SceneNode* node) {
    return selectedNodeUID.has_value() && selectedNodeUID.value() == node->GetUID();
}
