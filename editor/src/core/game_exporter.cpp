#include "game_exporter.h"

#include <algorithm>
#include <cctype>

#include <seika/utils/se_platform.h>

#include "../../../engine/src/core/core_info.h"

#include "editor_context.h"
#include "utils/file_system_helper.h"
#include "utils/console_logger.h"

#define GAME_EXPORTER_TMP_DIR_NAME "tmp_build"

namespace
{
    inline void StringToLower(std::string& string) {
        std::transform(string.begin(), string.end(), string.begin(),[](unsigned char c) {
            return std::tolower(c);
        });
    }

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

    // MacOS stuff

    struct PListInfoData {
        std::string cf_bundle_name; // game title
        std::string cf_bundle_executable; // executable name (e.g. game_title.exe)
        std::string cf_bundle_version = CRE_CORE_VERSION; // crescent version
        std::string ls_minimum_system_version = "10.15.0"; // minimum macOS version

        std::string GetFileString() const {
            return "";
        }
    };

    void CreateMacOSBundleDirectories(const std::filesystem::path buildPath) {}

    void GeneratePListInfoFile(const PListInfoData& data) {}
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    ConsoleLogger* consoleLogger = ConsoleLogger::Get();
    // 1. Fix up game title
    const std::string gameFileName = ConvertGameTitleToFileName(props.gameTitle);
    // 2. Remove old dir (if exists) and create new one
    const std::filesystem::path tempBuildPath = props.tempPath + SE_PLATFORM_PATH_SEPARATOR_STRING + GAME_EXPORTER_TMP_DIR_NAME;
    auto returnStatus = FileSystemHelper::RecreateDirectory(tempBuildPath.string());
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
    // 4. Create .pck file from project files
    const std::string zipName = gameFileName + ".pck";
    FileSystemHelper::ZipDirectory(zipName, tempBuildPath);
    // 5. Remove all project files (everything except .pck)
    const std::filesystem::path zipPath = std::filesystem::path(tempBuildPath) / zipName;
    FileSystemHelper::DeleteAllInDirectory(tempBuildPath, { zipPath });
    // 6. Create game binary (runtime) by copying the engine binary into the temp build folder
    const std::string engineBinaryName = std::string("crescent_engine")  + std::string(EDITOR_ENGINE_EXTENSION);
    const auto engineBinaryPath = std::filesystem::path(props.binPath) / engineBinaryName;
    const auto gameBinaryDest = tempBuildPath / std::filesystem::path(gameFileName + EDITOR_ENGINE_EXTENSION);
    FileSystemHelper::CopyFile(engineBinaryPath, gameBinaryDest);
    // 7. OS specific stuff, Window need dlls and MacOS needs to create the app bundle
    switch (props.platform) {
        case Platform::Undefined:
            // TODO: Error
            break;
        case Platform::Windows: {
            // Copy all necessary dlls
            FileSystemHelper::CopyFilesRecursivelyWithExtension(props.binPath, tempBuildPath, { ".dll" });
            break;
        }
        case Platform::Linux:
            break;
        case Platform::MacOS: {
            // Create app bundle folders
            CreateMacOSBundleDirectories(tempBuildPath);
            // Create PS Info file
            GeneratePListInfoFile({ .cf_bundle_name = gameFileName, .cf_bundle_executable = gameBinaryDest.filename().string() });
            // Move all files to macos folder
            break;
        }
    }
    // 8. Now that we have everything either create a zip or tar file and move it to specified archive path.
    const std::filesystem::path exportArchivePath = props.exportArchivePath;
    const std::filesystem::path exportArchiveName = exportArchivePath.filename();
    const std::filesystem::path buildArchivePath = std::filesystem::path(tempBuildPath) / exportArchiveName;
    FileSystemHelper::ZipDirectory(exportArchiveName.string(), tempBuildPath);
    FileSystemHelper::MoveFile(buildArchivePath, exportArchivePath);
    // 9. And finally we delete our temp build directory
    FileSystemHelper::DeleteDirectory(tempBuildPath);
}

GameExporter::Platform GameExporter::GetPlatformFromString(const std::string &platformString) {
    std::string platformStringLower = platformString;
    StringToLower(platformStringLower);
    if (platformStringLower == "windows") {
        return GameExporter::Platform::Windows;
    } else if (platformStringLower == "linux") {
        return GameExporter::Platform::Linux;
    } else if (platformStringLower == "macos") {
        return GameExporter::Platform::MacOS;
    }
    return GameExporter::Platform::Undefined;
}
