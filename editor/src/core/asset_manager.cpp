#include "asset_manager.h"

void AssetManager::Initialize() {
    if (!isInitialized) {
        rbe_asset_manager_initialize();
        isInitialized = true;
    }
}

void AssetManager::Finalize() {
    if (isInitialized) {
        rbe_asset_manager_finalize();
        isInitialized = false;
    }
}

// Texture
Texture* AssetManager::LoadTexture(const char* fileName, const char* key) {
    return rbe_asset_manager_load_texture(fileName, key);
}

Texture* AssetManager::GetTexture(const char* key) {
    return rbe_asset_manager_get_texture(key);
}

bool AssetManager::HasTexture(const char* key) const {
    return rbe_asset_manager_has_texture(key);
}

// Font
Font* AssetManager::LoadFont(const char* fileName, const char* key, int size) {
    return rbe_asset_manager_load_font(fileName, key, size);
}

Font* AssetManager::GetFont(const char* key) {
    return rbe_asset_manager_get_font(key);
}

bool AssetManager::HasFont(const char* key) const {
    return rbe_asset_manager_has_font(key);
}

// Audio Source
RBEAudioSource* AssetManager::LoadAudioSource(const char* fileName, const char* key) {
    return rbe_asset_manager_load_audio_source_wav(fileName, key);
}

RBEAudioSource* AssetManager::GetAudioSource(const char* key) {
    return rbe_asset_manager_get_audio_source(key);
}

bool AssetManager::HasAudioSource(const char* key) const {
    return rbe_asset_manager_has_audio_source(key);
}
