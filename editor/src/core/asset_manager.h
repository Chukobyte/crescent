#pragma once

#include "utils/singleton.h"

#include "../engine/src/core/asset_manager.h"
#include "../engine/src/core/rendering/texture.h"
#include "../engine/src/core/rendering/font.h"
#include "../engine/src/core/audio/audio.h"

// C++ Wrapper for the engine's asset manager
class AssetManager : public Singleton<AssetManager> {
  public:
    explicit AssetManager(singleton) {}
    void Initialize();
    void Finalize();
    // Texture
    Texture* LoadTexture(const char* fileName, const char* key);
    Texture* GetTexture(const char* key);
    bool HasTexture(const char* key) const;
    // Font
    Font* LoadFont(const char* fileName, const char* key, int size);
    Font* GetFont(const char* key);
    bool HasFont(const char* key) const;
    // Audio Source
    RBEAudioSource* LoadAudioSource(const char* fileName, const char* key);
    RBEAudioSource* GetAudioSource(const char* key);
    bool HasAudioSource(const char* key) const;
  private:
    bool isInitialized = false;
};
