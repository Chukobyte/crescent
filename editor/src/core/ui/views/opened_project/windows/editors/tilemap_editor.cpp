#include "tilemap_editor.h"

#include <format>

#include <imgui.h>

#include <seika/rendering/renderer.h>

#include "../engine/src/core/game_properties.h"

#include "../../../../../scene/scene_manager.h"
#include "../../../../../asset_manager.h"
#include "../../../../../project_properties.h"

namespace {
    SkaVector2i GetMouseTileCoords(const SkaSize2Di& tileSize) {
        const auto* gameProperties = ProjectProperties::Get();
        const ImVec2 mousePos = ImGui::GetMousePos();
        const ImVec2 windowPos = ImGui::GetWindowPos();
        const ImVec2 windowSize = ImGui::GetWindowSize();
        const ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
        // Calculate scaling factors based on the difference between actual window size and game resolution.
        const SkaSize2D gameResolution = gameProperties->GetResolutionSize();
        const SkaVector2 scaleFactor = {
            gameResolution.w / windowSize.x,
            gameResolution.h / windowSize.y
        };
        const SkaVector2 relativeMousePos = {
            (mousePos.x - windowPos.x - contentRegionMin.x) * scaleFactor.x,
            (mousePos.y - windowPos.y - contentRegionMin.y) * scaleFactor.y
        };
        SkaVector2i tileCoords = {
            static_cast<int32>(relativeMousePos.x) / tileSize.w,
            static_cast<int32>(relativeMousePos.y) / tileSize.h
        };
        // TODO: Make a range based on something (e.g.  max map size)
        const SkaVector2i tilePosMax = {999999,999999 };
        tileCoords.x = ska_math_clamp_int(tileCoords.x, 0, tilePosMax.x - 1);
        tileCoords.y = ska_math_clamp_int(tileCoords.y, 0, tilePosMax.y - 1);

        return tileCoords;
    }
}

static TilemapComp* cachedComp = nullptr;
static SkaTexture* colorRectTexture = nullptr;

void TilemapEditor::Process(SceneNode* node, TilemapComp* tilemapComp) {
    bool startedThisFrame = false;
    if (!isProcessing) {
        isProcessing = true;
        startedThisFrame = true;
        if (!colorRectTexture) {
            colorRectTexture = ska_texture_create_solid_colored_texture(1, 1, 255);
        }
    }
    selectedNodeUID = node->GetUID();
    cachedComp = tilemapComp;

    if (ImGui::IsMouseClicked(0) && !startedThisFrame && cachedState.isWindowFocused && cachedState.isWindowHovered) {
        const auto* tileCoords = &cachedState.mouseTileCoord;
        auto tilemap = cachedComp->GetInternalComp().tilemap;
        const auto tileData = cre_tilemap_get_tile_data(tilemap, tileCoords);
        // TODO: Do this better...
        static const auto renderCoords = SkaVector2i{ .x = 0, .y = 0 }; // TODO: Take in render coords
        if (tileData) {
            cre_tilemap_set_tile_active(tilemap, tileCoords, !tileData->isActive);
        } else {
            cre_tilemap_set_tile_render_coord(tilemap, tileCoords, &renderCoords);
        }
        cre_tilemap_commit_active_tile_changes(tilemap);
    }
}

void TilemapEditor::End() {
    if (isProcessing) {
        isProcessing = false;
        cachedState.Reset();
        selectedNodeUID.reset();
    }
}

bool TilemapEditor::IsNodeSelected(SceneNode* node) {
    return selectedNodeUID.has_value() && selectedNodeUID.value() == node->GetUID();
}

std::vector<ImGuiHelper::FontRenderTarget> TilemapEditor::GetFontRenderTargets() {
    if (isProcessing) {
        UpdateCachedState();
        const auto& tileCoords = cachedState.mouseTileCoord;
        const std::string formattedText = std::format("Tile Coords: ({}, {})", tileCoords.x, tileCoords.y);

        return {
                {
                    .font = AssetManager::Get()->GetFont(CRE_DEFAULT_FONT_ASSET.uid),
                    .text = formattedText,
                    .position = SkaVector2{ 16.0f, 16.0f },
                    .scale = 1.0f,
                    .color = SKA_COLOR_WHITE,
                    .zIndex = SKA_RENDERER_MAX_Z_INDEX
                }
        };
    }
    return {};
}

std::vector<ImGuiHelper::TextureRenderTarget> TilemapEditor::GetTextureRenderTargets() {
    if (isProcessing && ImGui::IsWindowFocused()) {
        UpdateCachedState();
        const auto& tileCoords = cachedState.mouseTileCoord;
        const auto tileSize = cachedComp->GetTileSize();
        return {
                {
                    .texture = colorRectTexture,
                    .sourceRect = { .x = 0.0f, .y = 0.0f, .w = 1.0f, .h = 1.0f },
                    .destSize = { .w = (float)tileSize.w, .h = (float)tileSize.h },
                    .color = SKA_COLOR_WHITE,
                    .flipH = false,
                    .flipV = false,
                    .zIndex = SKA_RENDERER_MAX_Z_INDEX,
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

void TilemapEditor::UpdateCachedState() {
    cachedState.isWindowFocused = ImGui::IsWindowFocused();
    cachedState.isWindowHovered = ImGui::IsWindowHovered();
    if (cachedState.isWindowFocused && cachedState.isWindowHovered) {
        cachedState.mouseTileCoord = GetMouseTileCoords(cachedComp->GetTileSize());
    }
}
