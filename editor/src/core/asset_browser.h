#pragma once

#include "utils/singleton.h"

#include "SquidTasks/Task.h"

class AssetBrowser : public Singleton<AssetBrowser> {
  public:
    AssetBrowser(singleton) {}

    Task<> UpdateFileAndDirectoryCache();

  private:
    void RefreshCache();
};
