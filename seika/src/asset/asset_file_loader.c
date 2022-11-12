#include "asset_file_loader.h"

#include "../data_structures/se_hash_map_string.h"

SEAssetFileLoaderReadMode globalReadMode = SEAssetFileLoaderReadMode_DISK;
SEStringHashMap* archiveAssetMap = NULL;

void sf_asset_file_loader_initialize() {
    archiveAssetMap = se_string_hash_map_create(8);
}

void sf_asset_file_loader_finalize() {
    se_string_hash_map_destroy(archiveAssetMap);
}

bool sf_asset_file_loader_load_archive(const char* filePath) {
    // TODO: Load archive into 'archiveAssetMap'
    return true;
}

void sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode readMode) {
    globalReadMode = readMode;
}

SEAssetFileLoaderReadMode sf_asset_file_loader_get_read_mode() {
    return globalReadMode;
}

SEArchiveFileAsset* sf_asset_file_loader_get_asset(const char* path) {
    if (se_string_hash_map_has(archiveAssetMap, path)) {
        return (SEArchiveFileAsset*) se_string_hash_map_get(archiveAssetMap, path);
    }
    return NULL;
}
