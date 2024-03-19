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
    [[nodiscard]] std::vector<ImGuiHelper::FontRenderTarget> GetFontRenderTargets() const;
    [[nodiscard]] std::vector<ImGuiHelper::TextureRenderTarget> GetTextureRenderTargets() const;
    bool IsNodeSelected(SceneNode* node);

private:
    bool isProcessing = false;
    std::optional<unsigned int> selectedNodeUID;
    static SKAVector2i GetMouseTileCoords();
};
