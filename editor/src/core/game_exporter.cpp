#include "game_exporter.h"

#include <algorithm>
#include <cctype>

#include <seika/utils/se_platform.h>

#include "utils/file_system_helper.h"
#include "utils/console_logger.h"

#define GAME_EXPORTER_TMP_DIR_NAME "tmp_build"

namespace
{
    std::string ConvertGameTitleToFileName(const std::string& gameTitle) {
        std::string fileName = gameTitle;
        // Convert the string to lowercase
        std::transform(fileName.begin(), fileName.end(), fileName.begin(),[](unsigned char c) {
            return std::tolower(c);
        });
        // Convert white space and dashes to underscores
        for (char& c: fileName) {
            if (c == ' ' || c == '-') {
                c = '_';
            }
        }
        return std::move(fileName);
    }
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    ConsoleLogger* consoleLogger = ConsoleLogger::Get();
    // 1. Fix up game title
    const std::string gameFileName = ConvertGameTitleToFileName(props.gameTitle);
    // 2. Remove old dir (if exists) and create new one
    const std::string tempBuildPath = props.tempPath + SE_PLATFORM_PATH_SEPARATOR_STRING + GAME_EXPORTER_TMP_DIR_NAME;
    auto returnStatus = FileSystemHelper::RecreateDirectory(tempBuildPath);
    if (!returnStatus) {
        printf("%s", returnStatus.errorCode.message().c_str());
        consoleLogger->AddEntry(returnStatus.errorCode.message());
    }
    // 3. Copy project files
    if (!FileSystemHelper::DoesDirectoryExist(props.projectPath)) {
        const std::string errorMessage = props.projectPath + " doesn't exist!";
        printf("%s", errorMessage.c_str());
        consoleLogger->AddEntry(errorMessage);
    }
    FileSystemHelper::CopyFilesRecursively(props.projectPath, tempBuildPath);
    // 4. Create .pck file
    const std::string zipName = gameFileName + ".pck";
    FileSystemHelper::ZipDirectory(zipName, tempBuildPath);
    // 5. Remove all files (except for .pck)
    const std::filesystem::path zipPath = std::filesystem::path(tempBuildPath) / zipName;
    FileSystemHelper::DeleteAllInDirectory(tempBuildPath, { zipPath });
    // 6. OS specific files, Window need dlls and MacOS needs to create the app bundle
    // 7. Now that we have everything either create a zip or tar file.
}
