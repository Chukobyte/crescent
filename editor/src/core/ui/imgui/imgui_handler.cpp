#include "imgui_handler.h"

#include <functional>
#include <utility>

#include "imgui.h"

#include <seika/seika.h>

// UIs
#include "../project_manager_ui.h"

#include "../opened_project_ui.h"
#include "../../editor_context.h"
#include "../../project_properties.h"

namespace {
    struct EditorScopeGuard {
        explicit EditorScopeGuard(std::function<void()> func) : guardFunc(std::move(func)) {}

        ~EditorScopeGuard() {
            if (guardFunc) {
                guardFunc();
            }
        }

        void Reset() {
            guardFunc = nullptr;
        }

        void ExecuteAndReset() {
            if (guardFunc) {
                guardFunc();
                guardFunc = nullptr;
            }
        }

    private:
        std::function<void()> guardFunc;
    };

    class EditorFPSCapture {
    public:
        float CaptureFPS(float startTime, float endTime) {
            const float elapsedTime = endTime - startTime;
            currentFPS = 1000.0f / elapsedTime;
            return currentFPS;
        }

        [[nodiscard]] float GetFPS() const { return currentFPS; }

    private:
        float currentFPS = 0.0f;
    };
}

void ImGuiHandler::Process() {
    static EditorContext* editorContext = EditorContext::Get();
    static EditorFPSCapture fpsCapture;

    static auto lastFrameTime = sf_get_ticks();
    const auto currentFrameTime = sf_get_ticks();
    const auto editorDeltaTime = (static_cast<float>(currentFrameTime) - static_cast<float>(lastFrameTime)) / 1000.0f;

    // Capture fps when scope collapses
    auto CaptureFPSGuard = EditorScopeGuard([currentFrameTime, editorDeltaTime] {
        editorContext->currentFPS = fpsCapture.CaptureFPS(static_cast<float>(currentFrameTime), static_cast<float>(sf_get_ticks()));

        const auto simTargetFPS = static_cast<float>(ProjectProperties::Get()->targetFPS);
        const float simDeltaTime = editorDeltaTime * ((float)simTargetFPS / editorContext->currentFPS);

        editorContext->editorDeltaTime = editorDeltaTime;
        editorContext->simDeltaTime = simDeltaTime;
    });

    switch (editorContext->projectState) {
    case EditorProjectState::ProjectManager: {
        ProjectManagerUI::ProcessMenuBar();
        ProjectManagerUI::ProcessWindows();
        ProjectManagerUI::ProcessModalPopups();
        break;
    }
    case EditorProjectState::OpenedProject: {
        OpenedProjectUI::ProcessMenuBar();
        OpenedProjectUI::ProcessWindows();
        OpenedProjectUI::ProcessModalPopups();
        break;
    }
    default:
        break;
    }

    lastFrameTime = currentFrameTime;
}
