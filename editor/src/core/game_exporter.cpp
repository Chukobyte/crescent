#include "game_exporter.h"

#include <kuba_zip/zip.h>

#include <seika/utils/se_platform.h>

#include "utils/file_system_helper.h"

#define GAME_EXPORTER_TMP_DIR_NAME "tmp_build"

namespace
{
    std::string ConvertGameTitleToFileName(const std::string& gameTitle) {
        std::string fileName = gameTitle;
        for (char& c: fileName) {
            if (c == ' ' || c == '-') {
                c = '_';
            }
        }
        return std::move(fileName);
    }
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    // 1. Fix up game title
    const std::string gameFileName = ConvertGameTitleToFileName(props.gameTitle);
    // 2. Remove old dir (if exists) and create new one
    const std::string tempBuildPath = props.tempPath + SE_PLATFORM_PATH_SEPARATOR_STRING + GAME_EXPORTER_TMP_DIR_NAME;
    FileSystemHelper::RecreateDirectory(tempBuildPath);
    // 3. Copy project files
    FileSystemHelper::CopyFilesRecursively(props.projectPath, tempBuildPath);
    // 4. Create .pck file
    // 5. Remove all files (except for .pck)
    // 6. OS specific files, Window need dlls and MacOS needs to create the app bundle
    // 7. Now that we have everything either create a zip or tar file.
}
