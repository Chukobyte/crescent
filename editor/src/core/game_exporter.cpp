#include "game_exporter.h"

#include <Python.h>

#include "../seika/src/utils/logger.h"

#include "asset_browser.h"
#include "utils/file_system_helper.h"
#include "editor_context.h"

//namespace {
//void CopyAllFilesFromFileNode(const FileNode& fileNode, const std::string& copyRootPath, bool copyCurrentNode = true) {
//    static auto CopyFunc = [] (const FileNode& fn, const std::string& copyPath) {
//        const std::filesystem::path copyDest = copyPath + "/" + fn.GetRelativePath();
//        FileSystemHelper::CopyFile(fn.path, copyDest);
//        se_logger_debug("copying project file from '%s' to '%s'", fn.path.generic_string().c_str(),
//                        copyDest.generic_string().c_str());
//    };
//    if (copyCurrentNode) {
//        CopyFunc(fileNode, copyRootPath);
//    }
//    for (auto& dir : fileNode.directories) {
//        CopyAllFilesFromFileNode(dir, copyRootPath);
//    }
//    for (auto& dir : fileNode.files) {
//        CopyAllFilesFromFileNode(dir, copyRootPath);
//    }
//}
//} // namespace
//
//void GameExporter::Export(const GameExporter::ExportProperties &props) {
//    const std::string& gameTitle = props.gameTitle;
//    const std::string& exportName = props.exportName;
//    std::string gameTitleFileName = gameTitle;
//    std::transform(gameTitleFileName.begin(), gameTitleFileName.end(), gameTitleFileName.begin(), [](unsigned char c) {
//        if (c == ' ') {
//            return '_';
//        }
//        return (char) std::tolower(c);
//    });
//    const std::filesystem::path exportPath = props.exportPath;
//    const std::filesystem::path exportParentPath = exportPath.parent_path();
//    const std::filesystem::path projectPath = props.projectPath;
//    const std::filesystem::path binPath = props.binPath;
//    const std::filesystem::path tempPath = props.tempPath;
//    const std::filesystem::path fullExportPath = exportPath / exportName;
//    std::error_code ec;
//    // Validation
//    if (!std::filesystem::is_directory(projectPath) || !std::filesystem::is_directory(binPath)) {
//        se_logger_error("Failed to export to path '%s'!  Project or bin directory doesn't exist!",
//                        exportPath.generic_string().c_str());
//        return;
//    }
//    // TODO: Do the same for temp path and move files to the full export path
//    if (std::filesystem::is_directory(fullExportPath)) {
//        std::uintmax_t numberDeleted = std::filesystem::remove_all(exportPath, ec);
//        if (ec.value() != 0) {
//            se_logger_error("Export failed!  Error code: '%d', message = '%s'", ec.value(), ec.message().c_str());
//        }
//    }
//    se_logger_debug("export file path = '%s'\nproject file path = '%s'\nbin file path = '%s'\ntemp file path = '%s'",
//                    exportPath.generic_string().c_str(), projectPath.generic_string().c_str(),
//                    binPath.generic_string().c_str(), tempPath.generic_string().c_str());
//    std::filesystem::create_directory(fullExportPath, ec);
//    // Copy engine binary
//    const std::filesystem::path binarySourcePath = binPath / std::string(EDITOR_ENGINE_BINARY_NAME);
//    const std::filesystem::path binaryDestPath = fullExportPath / std::string(gameTitleFileName + EDITOR_ENGINE_EXTENSION);
//    FileSystemHelper::CopyFile(binarySourcePath, binaryDestPath);
//    // Copy internal assets folder
//    const std::filesystem::path internalAssetsSourcePath = binPath / "assets";
//    const std::filesystem::path internalAssetsDestPath = fullExportPath / "assets";
//    FileSystemHelper::CopyFile(internalAssetsSourcePath, internalAssetsDestPath);
//    // Copy DLLs if windows
//#ifdef _WIN32
//    for (auto const& dir_entry : std::filesystem::directory_iterator {binPath}) {
//        if (dir_entry.is_regular_file() && dir_entry.path().extension().string() == ".dll") {
//            const std::filesystem::path dllDestPath = fullExportPath / dir_entry.path().filename();
//            FileSystemHelper::CopyFile(dir_entry.path(), dllDestPath);
//            se_logger_debug("DLL source = '%s', dest = '%s'", dir_entry.path().generic_string().c_str(),
//                            dllDestPath.generic_string().c_str());
//        }
//    }
//#endif
//    // Copy valid project files.  Using asset browser's file cache for now.
//    static AssetBrowser* assetBrowser = AssetBrowser::Get();
//    CopyAllFilesFromFileNode(assetBrowser->fileCache.rootNode, fullExportPath.string(), false);
//}

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    const std::string& gameTitle = props.gameTitle;

    PyRun_SimpleString("");
//    struct ExportProperties {
//        std::string gameTitle;
//        std::string exportName;
//        std::string exportPath;
//        std::string projectPath;
//        std::string binPath;
//        std::string tempPath;
//    };
}
