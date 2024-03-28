#pragma once

#include "utils/singleton.h"

#include <seika/asset/asset_manager.h>
#include <seika/rendering/texture.h>
#include <seika/rendering/font.h>
#include <seika/audio/audio.h>

// C++ Wrapper for the engine's asset manager
class AssetManager : public Singleton<AssetManager> {
  public:
    explicit AssetManager(singleton) {}
    void Initialize();
    void Finalize();
    void ClearContents();
    void RefreshFromProperties(class ProjectProperties* projectProperties);
    // Texture
    SkaTexture* LoadTexture(const char* fileName, const char* key);
    SkaTexture* GetTexture(const char* key);
    SkaTexture* GetTextureSafe(const char* key);
    bool HasTexture(const char* key) const;
    // Font
    SkaFont* LoadFont(const char* fileName, const char* key, int size, bool applyNearestNeighbor);
    SkaFont* GetFont(const char* key);
    bool HasFont(const char* key) const;
    // Audio Source
    SkaAudioSource* LoadAudioSource(const char* fileName, const char* key);
    SkaAudioSource* GetAudioSource(const char* key);
    bool HasAudioSource(const char* key) const;
};
