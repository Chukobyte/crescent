#pragma once

#include "utils/singleton.h"

#include "../seika/src/asset/asset_manager.h"
#include "../seika/src/rendering/texture.h"
#include "../seika/src/rendering/font.h"
#include "../seika/src/audio/audio.h"

// C++ Wrapper for the engine's asset manager
class AssetManager : public Singleton<AssetManager> {
  public:
    explicit AssetManager(singleton) {}
    void Initialize();
    void Finalize();
    void ClearContents();
    void RefreshFromProperties(class ProjectProperties* projectProperties);
    // Texture
    Texture* LoadTexture(const char* fileName, const char* key);
    Texture* GetTexture(const char* key);
    Texture* GetTextureSafe(const char* key);
    bool HasTexture(const char* key) const;
    // Font
    SEFont* LoadFont(const char* fileName, const char* key, int size);
    SEFont* GetFont(const char* key);
    bool HasFont(const char* key) const;
    // Audio Source
    SEAudioSource* LoadAudioSource(const char* fileName, const char* key);
    SEAudioSource* GetAudioSource(const char* key);
    bool HasAudioSource(const char* key) const;
};
