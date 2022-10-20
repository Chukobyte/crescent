#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void se_asset_manager_initialize();
void se_asset_manager_finalize();
// --- Texture --- //
struct Texture* se_asset_manager_load_texture(const char* fileName, const char* key);
struct Texture* se_asset_manager_get_texture(const char* key);
bool se_asset_manager_has_texture(const char* key);
// --- Font --- //
struct Font* se_asset_manager_load_font(const char* fileName, const char* key, int size);
struct Font* se_asset_manager_get_font(const char* key);
bool se_asset_manager_has_font(const char* key);
// -- Audio Source --- //
struct SEAudioSource* se_asset_manager_load_audio_source_wav(const char* fileName, const char* key);
struct SEAudioSource* se_asset_manager_get_audio_source(const char* key);
bool se_asset_manager_has_audio_source(const char* key);

#ifdef __cplusplus
}
#endif
