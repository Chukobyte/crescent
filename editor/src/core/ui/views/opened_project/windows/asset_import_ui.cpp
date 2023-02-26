#include "asset_import_ui.h"
#include "../../../../asset_browser.h"
#include "../../../../project_properties.h"
#include "../../../../file_creation/config_file_creator.h"

ImGuiHelper::Window OpenedProjectUI::Windows::GetAssetImportWindow() {
    ImGuiHelper::Window assetImportWindow = {
        .name = "Import",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static AssetBrowser* assetBrowser = AssetBrowser::Get();
            static unsigned int selectedIndex = 0;
            if (assetBrowser->selectedFileNode.has_value() && assetBrowser->selectedFileNode->regularFileType != FileNodeRegularFileType::Invalid) {
                const FileNode& selectedFileNode = assetBrowser->selectedFileNode.value();
                ImGui::Text("file: %s", selectedFileNode.path.filename().string().c_str());
                if (selectedFileNode.regularFileType == FileNodeRegularFileType::Texture) {
                    static std::string selectedWrapS = "clamp_to_border";
                    static ImGuiHelper::ComboBox wrapSComboBox("Wrap S", { "clamp_to_border", "repeat" }, [](const char* newItem) {
                        selectedWrapS = newItem;
                    });

                    static std::string selectedWrapT = "clamp_to_border";
                    static ImGuiHelper::ComboBox wrapTComboBox("Wrap T", { "clamp_to_border", "repeat" }, [](const char* newItem) {
                        selectedWrapT = newItem;
                    });

                    static bool selectedNearestNeighbor = true;
                    static ImGuiHelper::CheckBox nearestNeighborCheckBox("Nearest Neighbor", selectedNearestNeighbor);

                    // TODO: Use something else other than index since things can be thrown off if a file is added, deleted, renamed/replaced
                    if (selectedIndex != assetBrowser->selectedFileNode->index) {
                        selectedIndex = assetBrowser->selectedFileNode->index;
                        ProjectProperties* properties = ProjectProperties::Get();
                        auto& textureAsset = properties->GetTextureAsset(selectedFileNode.GetRelativePath());
                        wrapSComboBox.SetSelected(textureAsset.wrap_s);
                        wrapTComboBox.SetSelected(textureAsset.wrap_t);
                        selectedNearestNeighbor = textureAsset.nearestNeighbor;
                    }

                    ImGuiHelper::BeginComboBox(wrapSComboBox);
                    ImGuiHelper::BeginComboBox(wrapTComboBox);
                    ImGuiHelper::BeginCheckBox(nearestNeighborCheckBox);

                    ImGui::Separator();
                    if (ImGui::Button("Reimport")) {
                        ProjectProperties* projectProperties = ProjectProperties::Get();
                        const TextureAsset updatedTextureAsset = TextureAsset(selectedFileNode.GetRelativePath(), selectedWrapS, selectedWrapT, selectedNearestNeighbor);
                        projectProperties->UpdateTextureAsset(updatedTextureAsset);
                        ConfigFileCreator::GenerateConfigFile(ProjectProperties::Get());
                    }
                }
                // TODO: Add audio source properties when they exist...
            }
        },
        .position = ImVec2{ 150.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    return assetImportWindow;
}
