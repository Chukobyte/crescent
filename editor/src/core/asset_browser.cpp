#include "asset_browser.h"

#include "editor_context.h"
#include "utils/file_system_helper.h"
#include "../../../engine/src/core/utils/logger.h"

#include <filesystem>

using namespace Squid;

Task<> AssetBrowser::UpdateFileSystemCache() {
    while (true) {
        co_await WaitSeconds(10.0f, EditorContext::Time);

        RefreshCache();
    }
}

void AssetBrowser::RefreshCache() {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDirectory());
    rbe_logger_debug("root name: %s", projectRootDir.root_name().c_str());
    rbe_logger_debug("root path: %s", projectRootDir.string().c_str());

    for (auto const& dir_entry : std::filesystem::directory_iterator{projectRootDir}) {
        rbe_logger_debug("dir entry path: '%s'", dir_entry.path().string().c_str());
    }
}
