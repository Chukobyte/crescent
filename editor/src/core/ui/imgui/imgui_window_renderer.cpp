#include "imgui_window_renderer.h"

#include "imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include "../engine/src/core/rendering/renderer.h"
#include "../engine/src/core/rendering/frame_buffer.h"
#include "../engine/src/core/memory/rbe_mem.h"

#include "../../editor_context.h"

Color backgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };

Texture* screenTexture = nullptr;

namespace {
TransformModel2D GetDefaultGlobalTransform() {
    TransformModel2D transformModel2D = {
        .position = { 400.0f, 300.0f },
        .scale = { 4.0f, 4.0f },
        .rotation = 0.0f,
        .scaleSign = { 1.0f, 1.0f }
    };
    Transform2DComponent transform2DComponent = {
        .localTransform = {
            .position = { 400.0f, 300.0f },
            .scale = { 4.0f, 4.0f },
            .rotation = 0.0f
        }
    };
    transform2d_component_get_local_model_matrix(transformModel2D.model, &transform2DComponent);
    return transformModel2D;
}
} // namespace

void ImGuiHelper::WindowRenderer::Initialize() {
    rbe_renderer_initialize();
    screenTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
}

void ImGuiHelper::WindowRenderer::Render() {
    // Queue Test Sprite
    static Rect2 sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    static Size2D destSize = { 64.0f, 64.0f };
    static Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    static TransformModel2D globalTransform = GetDefaultGlobalTransform();
    // New frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(EditorContext::Get()->window);
    // Queue draw call within engine code paths
    rbe_renderer_queue_sprite_draw_call(screenTexture, sourceRect, destSize, color, false, false, &globalTransform);
    // Flush queued calls and render to framebuffer
    cre_renderer_process_and_flush_batches(&backgroundColor, false);
    // Add screen texture to draw list
    const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    const ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::GetWindowDrawList()->AddImage(
        (ImTextureID) cre_frame_buffer_get_color_buffer_texture(),
        cursorPos,
        ImVec2(cursorPos.x + windowSize.x, cursorPos.y + windowSize.y),
        ImVec2(0.0f, 1.0f),
        ImVec2(1.0f, 0.0f)
    );
}

void ImGuiHelper::WindowRenderer::Finalize() {
    if (screenTexture) {
        RBE_MEM_FREE(screenTexture);
    }
}
