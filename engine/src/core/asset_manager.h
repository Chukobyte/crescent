#pragma once

#include <stdbool.h>

void rbe_asset_manager_initialize();
void rbe_asset_manager_finalize();
// --- Texture --- //
struct Texture* rbe_asset_manager_load_texture(const char* fileName, const char* key);
struct Texture* rbe_asset_manager_get_texture(const char* key);
bool rbe_asset_manager_has_texture(const char* key);
// -- Audio Source --- //
struct RBEAudioSource* rbe_asset_manager_load_audio_source_wav(const char* fileName, const char* key);
struct RBEAudioSource* rbe_asset_manager_get_audio_source(const char* key);
bool rbe_asset_manager_has_audio_source(const char* key);
