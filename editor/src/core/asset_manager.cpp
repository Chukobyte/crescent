#include "asset_manager.h"

#include "../engine/src/core/game_properties.h"

#include "asset_browser.h"
#include "editor_context.h"
#include "project_properties.h"

static AssetBrowserRefreshSubscriberHandle subscriberHandle = 0;
static bool isInitialized = false;

// TODO: Remove assets from refresh functions
namespace {
void RefreshAllTextures(AssetManager* assetManager, const std::vector<FileNode>& fileNodes) {
    for (const FileNode& textureFile : fileNodes) {
        const std::string relativePath = textureFile.GetRelativePath();
        if (!assetManager->HasTexture(relativePath.c_str())) {
            assetManager->LoadTexture(textureFile.path.string().c_str(), relativePath.c_str());
        }
    }
}

void RefreshAllAudioSources(AssetManager* assetManager, const std::vector<FileNode>& fileNodes) {
    for (const FileNode& audioSourceFile : fileNodes) {
        const std::string relativePath = audioSourceFile.GetRelativePath();
        if (!assetManager->HasAudioSource(relativePath.c_str())) {
            assetManager->LoadAudioSource(audioSourceFile.path.string().c_str(), relativePath.c_str());
        }
    }
}
} // namespace

void AssetManager::Initialize() {
    if (!isInitialized) {
        se_asset_manager_initialize();
        AssetBrowser* assetBrowser = AssetBrowser::Get();
        // Load default font
        EditorContext* editorContext = EditorContext::Get();
        LoadFont(editorContext->GetEngineDefaultFontPath().c_str(), CRE_DEFAULT_FONT_ASSET.uid, CRE_DEFAULT_FONT_ASSET.size, CRE_DEFAULT_FONT_ASSET.applyNearestNeighbor);

        subscriberHandle = assetBrowser->RegisterRefreshCallback([this, assetBrowser](const FileNode& rootNode) {
            // Textures
            if (assetBrowser->fileCache.HasFilesWithExtension(".png")) {
                RefreshAllTextures(this, assetBrowser->fileCache.GetFilesWithExtension(".png"));
            }
            // Audio Sources
            if (assetBrowser->fileCache.HasFilesWithExtension(".wav")) {
                RefreshAllAudioSources(this, assetBrowser->fileCache.GetFilesWithExtension(".wav"));
            }
        });
        isInitialized = true;
    }
}

void AssetManager::Finalize() {
    if (isInitialized) {
        se_asset_manager_finalize();
        AssetBrowser::Get()->UnregisterRefreshCallback(subscriberHandle);
        isInitialized = false;
    }
}

void AssetManager::ClearContents() {
    // TODO: Implement...
}

void AssetManager::RefreshFromProperties(ProjectProperties* projectProperties) {
    // TODO: Remove ones that don't exist anymore...
    for (const FontAsset& fontAsset : projectProperties->assets.fonts) {
        if (!HasFont(fontAsset.uid.c_str())) {
            LoadFont(fontAsset.file_path.c_str(), fontAsset.uid.c_str(), fontAsset.size, fontAsset.applyNearestNeighbor);
        }
    }
    // Set wav sample rate for audio system
    se_audio_set_wav_sample_rate(projectProperties->audioWavSampleRate);
}

// Texture
SETexture* AssetManager::LoadTexture(const char* fileName, const char* key) {
    return se_asset_manager_load_texture(fileName, key);
}

SETexture* AssetManager::GetTexture(const char* key) {
    return se_asset_manager_get_texture(key);
}

SETexture* AssetManager::GetTextureSafe(const char *key) {
    return se_asset_manager_has_texture(key) ? se_asset_manager_get_texture(key) : nullptr;
}

bool AssetManager::HasTexture(const char* key) const {
    return se_asset_manager_has_texture(key);
}

// Font
SEFont* AssetManager::LoadFont(const char* fileName, const char* key, int size, bool applyNearestNeighbor) {
    return se_asset_manager_load_font(fileName, key, size, applyNearestNeighbor);
}

SEFont* AssetManager::GetFont(const char* key) {
    return se_asset_manager_get_font(key);
}

bool AssetManager::HasFont(const char* key) const {
    return se_asset_manager_has_font(key);
}

// Audio Source
SEAudioSource* AssetManager::LoadAudioSource(const char* fileName, const char* key) {
    return se_asset_manager_load_audio_source_wav(fileName, key);
}

SEAudioSource* AssetManager::GetAudioSource(const char* key) {
    return se_asset_manager_get_audio_source(key);
}

bool AssetManager::HasAudioSource(const char* key) const {
    return se_asset_manager_has_audio_source(key);
}
