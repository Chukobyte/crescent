#pragma once

#include <stddef.h>
#include <stdbool.h>

#include <glad/glad.h>

typedef enum SEAssetFileLoaderReadMode {
    SEAssetFileLoaderReadMode_DISK,
    SEAssetFileLoaderReadMode_ARCHIVE,
} SEAssetFileLoaderReadMode;

typedef struct SEArchiveFileAsset {
    void* buffer;
    size_t bufferSize;
} SEArchiveFileAsset;

typedef struct SEAssetFileImageData {
    unsigned char* data;
    GLsizei width;
    GLsizei height;
    int nrChannels;
} SEAssetFileImageData;

void sf_asset_file_loader_initialize();
void sf_asset_file_loader_finalize();
bool sf_asset_file_loader_load_archive(const char* filePath);
void sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode readMode);
SEAssetFileLoaderReadMode sf_asset_file_loader_get_read_mode();
SEArchiveFileAsset sf_asset_file_loader_get_asset(const char* path);
bool sf_asset_file_loader_is_asset_valid(SEArchiveFileAsset* fileAsset);

// Asset loading types
SEAssetFileImageData* sf_asset_file_loader_load_image_data(const char* filePath);
void sf_asset_file_loader_free_image_data(SEAssetFileImageData* data);
char* sf_asset_file_loader_read_file_contents_as_string(const char* filePath, size_t* size);
