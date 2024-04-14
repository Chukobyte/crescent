#include "tilemap_editor.h"

#include <imgui.h>

#include <seika/rendering/renderer.h>

#include "../engine/src/core/game_properties.h"

#include "../../../../../scene/scene_manager.h"
#include "../../../../../asset_manager.h"
#include "../../../../../project_properties.h"


static TilemapComp* cachedComp = nullptr;
static SkaTexture* colorRectTexture = nullptr;

struct TilemapCachedState
{
    SkaVector2i mouseTileCoord = SKA_VECTOR2I_ZERO;
    bool isWindowFocused = false;
    bool isWindowHovered = false;

    void Reset() {
        mouseTileCoord = SKA_VECTOR2I_ZERO;
        isWindowFocused = false;
        isWindowHovered = false;
    }
};

static TilemapCachedState cachedState;

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
    cachedState.isWindowFocused = ImGui::IsWindowFocused();
    cachedState.isWindowHovered = ImGui::IsWindowHovered();
    if (!cachedState.isWindowFocused || !cachedState.isWindowHovered) {
        return cachedState.mouseTileCoord;
    }

    const auto* gameProperties = ProjectProperties::Get();
    const auto tileSize = cachedComp->GetTileSize();
    const auto mousePos = ImGui::GetMousePos();
    const auto windowPos = ImGui::GetWindowPos();
    const auto windowSize = ImGui::GetWindowSize();
    const SkaVector2 rawWindowScroll = { .x = ImGui::GetScrollX(), .y = ImGui::GetScrollY() };
    const SkaSize2D gameResolution = { .w = (f32)gameProperties->resolutionWidth, .h = (f32)gameProperties->resolutionHeight };

    const SkaVector2 scaleFactor = {
        .x = windowSize.x / gameResolution.w,
        .y = windowSize.y / gameResolution.h,
    };

    const SkaVector2 windowScroll = { .x = rawWindowScroll.x * scaleFactor.x, .y = rawWindowScroll.y * scaleFactor.y };

    const SkaVector2 mousePosRelativeRaw = {
        .x = mousePos.x - windowPos.x + windowScroll.x,
        .y = mousePos.y - windowPos.y + windowScroll.y
    };

    SkaVector2 mousePosRelative = {
        .x = mousePosRelativeRaw.x / scaleFactor.x,
        .y = mousePosRelativeRaw.y / scaleFactor.y
    };
    mousePosRelative.x = ska_math_clamp_float(mousePosRelative.x, 0.0f, windowSize.x / scaleFactor.x);
    mousePosRelative.y = ska_math_clamp_float(mousePosRelative.y, 0.0f, windowSize.y / scaleFactor.y);

    SkaVector2i tileCoords = {
        .x = static_cast<int>(mousePosRelative.x / (f32)tileSize.w),
        .y = static_cast<int>(mousePosRelative.y / (f32)tileSize.h)
    };
    const SkaSize2Di tileLimit = { .w = static_cast<int>(windowSize.x / (f32)tileSize.w) * 2, .h = static_cast<int>(windowSize.y / (f32)tileSize.h) * 2 };
    tileCoords.x = ska_math_clamp_int(tileCoords.x, 0, tileLimit.w);
    tileCoords.y = ska_math_clamp_int(tileCoords.y, 0, tileLimit.h);

    cachedState.mouseTileCoord = tileCoords;

    return tileCoords;
}
