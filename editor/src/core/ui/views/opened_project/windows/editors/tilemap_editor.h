#pragma once

#include <optional>

#include "../../../../../components/component.h"
#include "../../../../../utils/singleton.h"
#include "../../../../imgui/imgui_window_renderer.h"

class SceneNode;

class TilemapEditor : public Singleton<TilemapEditor> {
public:
    explicit TilemapEditor(singleton) {}
    void Process(SceneNode* node, TilemapComp* tilemapComp);
    void End();
    [[nodiscard]] std::vector<ImGuiHelper::FontRenderTarget> GetFontRenderTargets();
    [[nodiscard]] std::vector<ImGuiHelper::TextureRenderTarget> GetTextureRenderTargets();
    bool IsNodeSelected(SceneNode* node);

private:
    bool isProcessing = false;
    std::optional<unsigned int> selectedNodeUID;

    struct TilemapCachedState {
        SkaVector2i mouseTileCoord = SKA_VECTOR2I_ZERO;
        bool isWindowFocused = false;
        bool isWindowHovered = false;

        void Reset() {
            mouseTileCoord = SKA_VECTOR2I_ZERO;
            isWindowFocused = false;
            isWindowHovered = false;
        }
    };

    TilemapCachedState cachedState;

    void UpdateCachedState();
};
