#include "game_exporter.h"

#include <algorithm>
#include <cctype>

#include <seika/file_system.h>
#include <seika/platform.h>
#include <seika/assert.h>

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

        [[nodiscard]] std::string GetFileString() const {
            std::string fileString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            fileString += "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
            fileString += "<plist version=\"1.0\">\n";
            fileString += "    <dict>\n";
            fileString += "        <key>CFBundleName</key>\n";
            fileString += "        <string>" + cf_bundle_name + "</string>\n";
            fileString += "        <key>CFBundleVersion</key>\n";
            fileString += "        <string>" + cf_bundle_version + "</string>\n";
            fileString += "        <key>CFBundleExecutable</key>\n";
            fileString += "        <string>" + cf_bundle_executable + "</string>\n";
            fileString += "        <key>LSMinimumSystemVersion</key>\n";
            fileString += "        <string>" + ls_minimum_system_version + "</string>\n";
            fileString += "    </dict>\n";
            fileString += "</plist>\n";

            return fileString;
        }
    };

    struct MacOSExportPaths {
        std::filesystem::path appBundlePath;
        std::filesystem::path contentsPath;
        std::filesystem::path macOSPath;
        std::filesystem::path resourcesPath;
        std::filesystem::path plistInfoPath;
    };

    MacOSExportPaths GenerateMacOSExportData(const std::filesystem::path& buildPath, const std::string& gameBinaryName) {
        const std::filesystem::path appBundlePath = buildPath / std::string(gameBinaryName + ".app");
        const std::filesystem::path contentsPath = appBundlePath / "Contents";
        const std::filesystem::path macOSPath = contentsPath / "MacOS";
        const std::filesystem::path resourcesPath = contentsPath / "Resources";
        const std::filesystem::path plistInfoPath = contentsPath / "Info.plist";

        return std::move(MacOSExportPaths{
            .appBundlePath = appBundlePath,
            .contentsPath = contentsPath,
            .macOSPath = macOSPath,
            .resourcesPath = resourcesPath,
            .plistInfoPath = plistInfoPath,
        });
    }

    void GeneratePListInfoFile(const std::filesystem::path& filePath, const PListInfoData& data) {
        const std::string infoFilePath = filePath.string().substr(2);
        ska_fs_write_to_file(infoFilePath.c_str(), data.GetFileString().c_str());
    }
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    const Platform platform = GetPlatformFromString(props.platform);
    const std::filesystem::path binPath = EditorContext::Get()->GetEngineBinPathByOS(props.platform);
    ConsoleLogger* consoleLogger = ConsoleLogger::Get();
    SKA_ASSERT(platform != Platform::Undefined);
    // 1. Fix up game title
    const std::string gameFileName = ConvertGameTitleToFileName(props.gameTitle);
    // 2. Remove old dir (if exists) and create new one
    const std::filesystem::path tempBuildPath = props.tempPath / GAME_EXPORTER_TMP_DIR_NAME;
    auto returnStatus = FileSystemHelper::RecreateDirectory(tempBuildPath.string());
    if (!returnStatus) {
        consoleLogger->AddEntry(returnStatus.errorCode.message());
    }
    // 3. Copy project files
    if (!FileSystemHelper::DoesDirectoryExist(props.projectPath)) {
        const std::string errorMessage = props.projectPath.string() + " doesn't exist!";
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
    const std::string runtimeBinaryExtension = platform == Platform::Windows ? ".exe" : "";
    const std::string engineBinaryName = "crescent_engine" + runtimeBinaryExtension;
    const auto engineBinaryPath = binPath / engineBinaryName;
    const auto gameBinaryDest = tempBuildPath / std::filesystem::path(gameFileName + runtimeBinaryExtension);
    FileSystemHelper::CopyFile(engineBinaryPath, gameBinaryDest);
    // 7. OS specific stuff, Window need dlls and MacOS needs to create the app bundle
    switch (platform) {
        case Platform::Undefined:
            // TODO: Error
            break;
        case Platform::Windows: {
            // Copy all necessary dlls
            FileSystemHelper::CopyFilesRecursivelyWithExtension(binPath, tempBuildPath, { ".dll" });
            break;
        }
        case Platform::Linux:
            break;
        case Platform::MacOS: {
            // Generate data first
            const auto exportData = GenerateMacOSExportData(tempBuildPath, gameBinaryDest.filename().string());
            // Create dirs
            FileSystemHelper::CreateDirectory(exportData.appBundlePath);
            FileSystemHelper::CreateDirectory(exportData.contentsPath);
            FileSystemHelper::CreateDirectory(exportData.macOSPath);
            FileSystemHelper::CreateDirectory(exportData.resourcesPath);
            // Create PS Info file
            GeneratePListInfoFile(exportData.plistInfoPath, { .cf_bundle_name = gameFileName, .cf_bundle_executable = gameBinaryDest.filename().string() });
            // Move all files to macos folder
            FileSystemHelper::ForEachFile(tempBuildPath, [&exportData](const std::filesystem::directory_entry& entry) {
                if (!std::filesystem::equivalent(entry.path(), exportData.appBundlePath)) {
                    FileSystemHelper::MoveFile(entry.path(), exportData.macOSPath / entry.path().filename());
                }
            });
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
    } else if (platformStringLower == "macosx") {
        return GameExporter::Platform::MacOS;
    }
    return GameExporter::Platform::Undefined;
}
