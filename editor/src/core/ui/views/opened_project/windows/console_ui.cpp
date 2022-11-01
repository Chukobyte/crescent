#include "console_ui.h"

#include "../../../../utils/console_logger.h"

ImGuiHelper::Window OpenedProjectUI::Windows::GetConsoleWindow() {
    ImGuiHelper::Window consoleWindow = {
        .name = "Console",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static ConsoleLogger* consoleLogger = ConsoleLogger::Get();
            if (ImGui::Button("Clear")) {
                consoleLogger->Clear();
            }
            ImGui::Separator();
            ImGui::BeginChild("ConsoleLogWindow", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextUnformatted(consoleLogger->GetText().c_str());
            ImGui::EndChild();
        },
        .position = ImVec2{ 200.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    return consoleWindow;
}
