#include "game_exporter.h"

#include <filesystem>

#include <seika/utils/se_platform.h>

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

    void RecreateDirectory(const std::string& dirPath) {
        try {
            // Delete if exists
            if (std::filesystem::exists(dirPath)) {
                std::filesystem::remove_all(dirPath);
            }
            // Now create the directory
            std::filesystem::create_directory(dirPath);
        } catch (const std::filesystem::filesystem_error& ex) {
            // TODO: print error
//            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void CopyFilesRecursively(const std::filesystem::path& source, const std::filesystem::path& dest) {
        try {
            // Create dir if it doesn't exist
            if (!std::filesystem::exists(dest)) {
                std::filesystem::create_directory(dest);
            }
            // Iterate through each entry now
            for (const auto& entry : std::filesystem::recursive_directory_iterator(source)) {
                // Check if the entry is a regular file and does not start with '.'
                if (std::filesystem::is_regular_file(entry.path()) && entry.path().filename().string()[0] != '.') {
                    // Construct the corresponding path in the destination directory
                    std::filesystem::path destPath = dest / entry.path().relative_path();
                    // Create necessary directories in the destination path
                    std::filesystem::create_directories(destPath.parent_path());
                    // Copy the file
                    std::filesystem::copy_file(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
                }
            }
        } catch (const std::filesystem::filesystem_error& ex) {
            // TODO: print error
        }
    }
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    // 1. Fix up game title
    const std::string gameFileName = ConvertGameTitleToFileName(props.gameTitle);
    // 2. Remove old dir (if exists) and create new one
    const std::string tempBuildPath = props.tempPath + SE_PLATFORM_PATH_SEPARATOR_STRING + GAME_EXPORTER_TMP_DIR_NAME;
    RecreateDirectory(tempBuildPath);
    // 3. Copy project files
    CopyFilesRecursively(props.projectPath, tempBuildPath);
    // 4. Copy engine 'bin' files
    // 5. Create .pck file
    // 6. Remove all files (except for .pck)
    // 7. OS specific files, Window need dlls and MaxOS needs to create the app bundle
    // 8. Now that we have everything either create a zip or tar file.
}
