#include "imgui_handler.h"

#include <iostream>

#include "imgui.h"

// UIs
#include "../opened_project_ui.h"
#include "../project_manager_ui.h"

#include "../../editor_context.h"

static bool isImGuiDemoEnabled = false;

void ImGuiHandler::Process() {
    static EditorContext* editorContext = EditorContext::Get();
    switch (editorContext->projectState) {
    case EditorProjectState::ProjectManager:
        ProjectManagerUI::ProcessMenuBar();
        ProjectManagerUI::ProcessModalPopups();
        ProjectManagerUI::ProcessWindows();
        break;
    case EditorProjectState::OpenedProject:
        OpenedProjectUI::ProcessMenuBar();
        OpenedProjectUI::ProcessModalPopups();
        OpenedProjectUI::ProcessWindows();
        break;
    default: break;
    }
    // standard demo window
    if (isImGuiDemoEnabled) {
        ImGui::ShowDemoWindow(&isImGuiDemoEnabled);
    }
}
