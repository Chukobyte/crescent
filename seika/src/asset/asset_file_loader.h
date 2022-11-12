#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef enum SEAssetFileLoaderReadMode {
    SEAssetFileLoaderReadMode_DISK,
    SEAssetFileLoaderReadMode_ARCHIVE,
} SEAssetFileLoaderReadMode;

typedef struct SEArchiveFileAsset {
    void* buffer;
    size_t bufferSize;
} SEArchiveFileAsset;

void sf_asset_file_loader_initialize();
void sf_asset_file_loader_finalize();
bool sf_asset_file_loader_load_archive(const char* filePath);
void sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode readMode);
SEAssetFileLoaderReadMode sf_asset_file_loader_get_read_mode();
SEArchiveFileAsset* sf_asset_file_loader_get_asset(const char* path);
