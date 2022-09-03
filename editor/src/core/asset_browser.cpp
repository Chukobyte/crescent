#include "asset_browser.h"

#include "editor_context.h"

using namespace Squid;

Task<> AssetBrowser::UpdateFileAndDirectoryCache() {
    while (true) {
        co_await WaitSeconds(10.0f, EditorContext::Time);

        RefreshCache();
    }
}

void AssetBrowser::RefreshCache() {
}
