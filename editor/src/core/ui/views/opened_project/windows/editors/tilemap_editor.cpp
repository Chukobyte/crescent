#include "tilemap_editor.h"

#include <imgui.h>

#include <seika/rendering/renderer.h>

#include "../engine/src/core/game_properties.h"

#include "../../../../../scene/scene_manager.h"
#include "../../../../../asset_manager.h"
#include "../../../../../project_properties.h"


static TilemapComp* cachedComp = nullptr;
static SkaTexture* colorRectTexture = nullptr;
SkaVector2i cachedMouseTileCoord = SKA_VECTOR2I_ZERO;
bool cachedIsWindowFocused = false;

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

    if (ImGui::IsMouseClicked(0) && !startedThisFrame && cachedIsWindowFocused) {
        const auto* tileCoords = &cachedMouseTileCoord;
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
        cachedIsWindowFocused = false;
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
                    .position = SkaVector2{ 16.0f, 16.0f },
                    .scale = 1.0f,
                    .color = SKA_COLOR_WHITE,
                    .zIndex = SKA_RENDERER_MAX_Z_INDEX
                }
        };
    }
//    return {};
}

std::vector<ImGuiHelper::TextureRenderTarget> TilemapEditor::GetTextureRenderTargets() const {
    if (isProcessing && ImGui::IsWindowFocused()) {
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

SkaVector2i TilemapEditor::GetMouseTileCoords() {
    cachedIsWindowFocused = ImGui::IsWindowFocused();
    if (!cachedIsWindowFocused) {
        return cachedMouseTileCoord;
    }

    const auto* gameProperties = ProjectProperties::Get();
    const auto tileSize = cachedComp->GetTileSize();
    const auto mousePos = ImGui::GetMousePos();
    const auto windowPos = ImGui::GetWindowPos();
    const auto windowSize = ImGui::GetWindowSize();
    const SkaSize2D gameResolution = { .w = (f32)gameProperties->resolutionWidth, .h = (f32)gameProperties->resolutionHeight };

    const SkaVector2 scaleFactor = {
        .x = windowSize.x / gameResolution.w,
        .y = windowSize.y / gameResolution.h,
    };

    const SkaVector2 mousePosRelative = {
        .x = (mousePos.x - windowPos.x) / scaleFactor.x,
        .y = (mousePos.x - windowPos.y) / scaleFactor.y
    };

    SkaVector2i tileCoords = {
        .x = static_cast<int>(mousePosRelative.x / (f32)tileSize.w),
        .y = static_cast<int>(mousePosRelative.y / (f32)tileSize.h)
    };
    static const int tempTileLimit = 256;
    tileCoords.x = ska_math_clamp_int(tileCoords.x, 0, tempTileLimit);
    tileCoords.y = ska_math_clamp_int(tileCoords.y, 0, tempTileLimit);

    cachedMouseTileCoord = tileCoords;

    return tileCoords;
}

//SkaVector2i TilemapEditor::GetMouseTileCoords() {
//    const auto* gameProperties = ProjectProperties::Get();
//    const auto tileSize = cachedComp->GetTileSize();
//    const auto mousePos = ImGui::GetMousePos();
//    const auto windowPos = ImGui::GetWindowPos();
//    const auto windowSize = ImGui::GetWindowSize();
//    const SkaVector2 scroll = { .x = ImGui::GetScrollX(), .y = ImGui::GetScrollY() };
////    const SkaVector2 zoom = {
////        .x = windowSize.x / static_cast<float>(gameProperties->resolutionWidth),
////        .y = windowSize.y / static_cast<float>(gameProperties->resolutionHeight)
////    };
//    const SkaVector2 zoom = SKA_VECTOR2_ONE;
//    // Adjust scroll values for zoom
//    const SkaVector2 adjustedScroll = { .x = scroll.x * zoom.x, .y = scroll.y * zoom.y };
//    // Calculate mouse position relative to the tilemap, adjusting for window position, scroll and zoom
//    const SkaVector2 mousePosRelative = {
//        .x = (mousePos.x - windowPos.x + adjustedScroll.x) / zoom.x,
//        .y = (mousePos.y - windowPos.y + adjustedScroll.y) / zoom.y
//    };
//    // Calculate tile coordinates
//    SkaVector2i tileCoords = {
//        .x = static_cast<int>(mousePosRelative.x / static_cast<float>(tileSize.w)),
//        .y = static_cast<int>(mousePosRelative.y / static_cast<float>(tileSize.h))
//    };
//    tileCoords.x = ska_math_clamp_int(tileCoords.x, 0, tileSize.w);
//    tileCoords.y = ska_math_clamp_int(tileCoords.y, 0, tileSize.h);
//    cachedMouseTileCoord = tileCoords;
//    return tileCoords;
//}
