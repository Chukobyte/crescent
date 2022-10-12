#include "imgui_window_renderer.h"

#include "imgui.h"

#include "../engine/src/core/rendering/renderer.h"
#include "../engine/src/core/rendering/frame_buffer.h"
#include "../engine/src/core/memory/rbe_mem.h"

Color backgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
Size2D windowSize = { 800.0f, 600.0f };

Texture* screenTexture = nullptr;

namespace {
TransformModel2D GetDefaultGlobalTransform() {
    TransformModel2D transformModel2D = {
        .position = { 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f },
        .rotation = 0.0f,
        .scaleSign = { 1.0f, 1.0f }
    };
    glm_mat4_identity(transformModel2D.model);
    return transformModel2D;
}
} // namespace

void ImGuiHelper::WindowRenderer::Initialize() {
    rbe_renderer_initialize();
    screenTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
}

void ImGuiHelper::WindowRenderer::Render() {
    // Queue Test Sprite
//    void rbe_renderer_queue_sprite_draw_call(Texture* texture, Rect2 sourceRect, Size2D destSize, Color color, bool flipX, bool flipY, TransformModel2D* globalTransform);
    static Rect2 sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    static Size2D destSize = { 64.0f, 64.0f };
    static Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    static TransformModel2D globalTransform = GetDefaultGlobalTransform();
    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        rbe_renderer_queue_sprite_draw_call(screenTexture, sourceRect, destSize, color, false, false, &globalTransform);
    }

    // Flush and render
//    cre_renderer_process_and_flush_batches(&backgroundColor);
//    ImGui::GetWindowDrawList()->AddImage(
//            (void*) cre_frame_buffer_get_color_buffer_texture(),
//            ImVec2(ImGui::GetCursorScreenPos()),
//            ImVec2(ImGui::GetCursorScreenPos().x + windowSize.w / 2.0f, ImGui::GetCursorScreenPos().y + windowSize.h / 2.0f),
//            ImVec2(0.0f, 1.0f),
//            ImVec2(1.0f, 0.0f)
//    );
}

void ImGuiHelper::WindowRenderer::Finalize() {
    if (screenTexture) {
        RBE_MEM_FREE(screenTexture);
    }
}
