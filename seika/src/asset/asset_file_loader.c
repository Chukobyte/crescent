#include "asset_file_loader.h"

#include <stb_image/stb_image.h>
#include <kuba_zip/zip.h>

#include "../data_structures/se_hash_map_string.h"
#include "../memory/se_mem.h"
#include "../utils/se_file_system_utils.h"
#include "../utils/se_string_util.h"

SEAssetFileLoaderReadMode globalReadMode = SEAssetFileLoaderReadMode_DISK;
struct zip_t* packageArchive = NULL;

void clear_package_archive() {
    if (packageArchive != NULL) {
        zip_entry_close(packageArchive);
        packageArchive = NULL;
    }
}

void sf_asset_file_loader_initialize() {}

void sf_asset_file_loader_finalize() {
    clear_package_archive();
}

bool sf_asset_file_loader_load_archive(const char* filePath) {
    if (se_fs_does_file_exist(filePath)) {
        clear_package_archive();
        packageArchive = zip_open(filePath, 0, 'r');
        return true;
    }
    return false;
}

void sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode readMode) {
    globalReadMode = readMode;
}

SEAssetFileLoaderReadMode sf_asset_file_loader_get_read_mode() {
    return globalReadMode;
}

SEArchiveFileAsset sf_asset_file_loader_get_asset(const char* path) {
    void* fileBuffer = NULL;
    size_t fileBufferSize;
    zip_entry_open(packageArchive, path);
    {
        zip_entry_read(packageArchive, &fileBuffer, &fileBufferSize);
    }
    return (SEArchiveFileAsset) {
        .buffer = fileBuffer,
        .bufferSize = fileBufferSize
    };
}

bool sf_asset_file_loader_is_asset_valid(SEArchiveFileAsset* fileAsset) {
    return fileAsset != NULL && fileAsset->buffer != NULL;
}

SEAssetFileImageData* sf_asset_file_loader_load_image_data(const char* filePath) {
    SEAssetFileImageData* imageData = NULL;
    stbi_set_flip_vertically_on_load(false);
    if (globalReadMode == SEAssetFileLoaderReadMode_DISK) {
        imageData = SE_MEM_ALLOCATE(SEAssetFileImageData);
        imageData->data = stbi_load(filePath, &imageData->width, &imageData->height, &imageData->nrChannels, 0);
    } else if (globalReadMode == SEAssetFileLoaderReadMode_ARCHIVE) {
        SEArchiveFileAsset fileAsset = sf_asset_file_loader_get_asset(filePath);
        if (sf_asset_file_loader_is_asset_valid(&fileAsset)) {
            imageData = SE_MEM_ALLOCATE(SEAssetFileImageData);
            imageData->data = stbi_load_from_memory(fileAsset.buffer, (int) fileAsset.bufferSize, &imageData->width, &imageData->height, &imageData->nrChannels, 0);
        }
    }
    return imageData;
}

void sf_asset_file_loader_free_image_data(SEAssetFileImageData* data) {
    stbi_image_free(data->data);
    SE_MEM_FREE(data);
}

char* sf_asset_file_loader_read_file_contents_as_string(const char* filePath, size_t* size) {
    char* fileString = NULL;
    size_t len = 0;
    if (globalReadMode == SEAssetFileLoaderReadMode_DISK) {
        if (se_fs_does_file_exist(filePath)) {
            fileString = se_fs_read_file_contents(filePath, &len);
        }
    } else if (globalReadMode == SEAssetFileLoaderReadMode_ARCHIVE) {
        SEArchiveFileAsset fileAsset = sf_asset_file_loader_get_asset(filePath);
        if (sf_asset_file_loader_is_asset_valid(&fileAsset)) {
            fileString = se_strdup((char*) fileAsset.buffer);
            len = fileAsset.bufferSize;
        }
    }
    if (size != NULL) {
        *size = len;
    }
    return fileString;
}
