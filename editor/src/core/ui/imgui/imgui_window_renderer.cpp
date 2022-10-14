#include "imgui_window_renderer.h"

#define CRE_DISABLE_RENDER_TO_SCREEN_FROM_FRAMEBUFFER // To disable the engine renderer from rendering to the screen

#include "imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include "../engine/src/core/rendering/renderer.h"
#include "../engine/src/core/rendering/frame_buffer.h"

#include "../../editor_context.h"

void ImGuiHelper::WindowRenderer::Initialize() {
    rbe_renderer_initialize();
}

void ImGuiHelper::WindowRenderer::Render(const std::vector<WindowRenderTarget>& renderTargets) {
    // New frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(EditorContext::Get()->window);
    // Queue draw calls within engine code paths
    for (auto& target : renderTargets) {
        rbe_renderer_queue_sprite_draw_call(target.texture, target.sourceRect, target.destSize, target.color, target.flipX, target.flipY, target.globalTransform);
    }
    // Flush queued calls and render to framebuffer
    static const Color backgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    cre_renderer_process_and_flush_batches(&backgroundColor);
    // Add screen texture from framebuffer to draw list
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

void ImGuiHelper::WindowRenderer::Finalize() {}
