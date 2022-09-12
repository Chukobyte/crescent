#include "game_exporter.h"

#include "../engine/src/core/utils/logger.h"

#include "asset_browser.h"
#include "utils/file_system_helper.h"

namespace {
void CopyAllFilesFromFileNode(const FileNode& fileNode, const std::string& copyRootPath, bool copyCurrentNode = true) {
    static auto CopyFunc = [] (const FileNode& fn, const std::string& copyPath) {
        const std::filesystem::path copyDest = copyPath + "/" + fn.GetRelativePath();
        FileSystemHelper::CopyFile(fn.path, copyDest);
        rbe_logger_debug("copying project file from '%s' to '%s'", fn.path.generic_string().c_str(), copyDest.generic_string().c_str());
    };
    if (copyCurrentNode) {
        CopyFunc(fileNode, copyRootPath);
    }
    for (auto& dir : fileNode.directories) {
        CopyAllFilesFromFileNode(dir, copyRootPath);
    }
    for (auto& dir : fileNode.files) {
        CopyAllFilesFromFileNode(dir, copyRootPath);
    }
}
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties &props) {
    const std::string& gameTitle = props.gameTitle;
    std::string gameTitleFileName = gameTitle;
    std::transform(gameTitleFileName.begin(), gameTitleFileName.end(), gameTitleFileName.begin(), [](unsigned char c) {
        if (c == ' ') {
            return '_';
        }
        return (char) std::tolower(c);
    });
    const std::filesystem::path exportPath = props.exportPath;
    const std::filesystem::path projectPath = props.projectPath;
    const std::filesystem::path binPath = props.binPath;
    const std::filesystem::path tempPath = props.tempPath;
    std::error_code ec;
    // Validation
    if (!std::filesystem::is_directory(projectPath) || !std::filesystem::is_directory(binPath)) {
        rbe_logger_error("Failed to export to path '%s'!  Project or bin directory doesn't exist!", exportPath.generic_string().c_str());
        return;
    }
    if (std::filesystem::is_directory(tempPath)) {
        std::uintmax_t numberDeleted = std::filesystem::remove_all(exportPath, ec);
        if (ec.value() != 0) {
            rbe_logger_error("Export failed!  Error code: '%d', message = '%s'", ec.value(), ec.message().c_str());
        }
    }
    rbe_logger_debug("export file path = '%s'\nproject file path = '%s'\nbin file path = '%s'\ntemp file path = '%s'",
                     exportPath.generic_string().c_str(), projectPath.generic_string().c_str(), binPath.generic_string().c_str(), tempPath.generic_string().c_str());
    std::filesystem::create_directory(tempPath, ec);
    // Copy engine binary
    const std::filesystem::path binarySourcePath = binPath / "crescent_engine.exe";
    const std::filesystem::path binaryDestPath = tempPath / std::string(gameTitleFileName + ".exe");
    FileSystemHelper::CopyFile(binarySourcePath, binaryDestPath);
    // Copy internal assets folder
    const std::filesystem::path internalAssetsSourcePath = binPath / "assets";
    const std::filesystem::path internalAssetsDestPath = tempPath / "assets";
    FileSystemHelper::CopyFile(internalAssetsSourcePath, internalAssetsDestPath);
    // Copy valid project files.  Using asset brower's file cache for now.
    static AssetBrowser* assetBrowser = AssetBrowser::Get();
    CopyAllFilesFromFileNode(assetBrowser->rootNode, tempPath.string(), false);
}
