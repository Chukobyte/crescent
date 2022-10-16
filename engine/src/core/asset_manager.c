#include "asset_manager.h"

#include "data_structures/rbe_hash_map_string.h"
#include "rendering/texture.h"
#include "rendering/font.h"
#include "audio/audio.h"
#include "audio/audio_manager.h"
#include "memory/rbe_mem.h"
#include "utils/rbe_assert.h"

#define INITIAL_ASSET_HASHMAP_SIZE 8

RBEStringHashMap* texturesMap = NULL;
RBEStringHashMap* fontMap = NULL;
RBEStringHashMap* audioSourceMap = NULL;

void rbe_asset_manager_initialize() {
    texturesMap = rbe_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
    fontMap = rbe_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
    audioSourceMap = rbe_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
}

void rbe_asset_manager_finalize() {
    rbe_string_hash_map_destroy(texturesMap);
    rbe_string_hash_map_destroy(fontMap);
    rbe_string_hash_map_destroy(audioSourceMap);
}

// --- Texture --- //
Texture* rbe_asset_manager_load_texture(const char* fileName, const char* key) {
    RBE_ASSERT(texturesMap != NULL);
    RBE_ASSERT_FMT(!rbe_string_hash_map_has(texturesMap, fileName), "Already loaded texture at file path '%'s!  Has key '%s'.", fileName, key);
    Texture* texture = rbe_texture_create_texture(fileName);
    rbe_string_hash_map_add(texturesMap, key, texture, sizeof(Texture));
    RBE_MEM_FREE(texture);
    texture = (Texture*) rbe_string_hash_map_get(texturesMap, key);
    return texture;
}

Texture* rbe_asset_manager_get_texture(const char* key) {
    return (Texture*) rbe_string_hash_map_get(texturesMap, key);
}

bool rbe_asset_manager_has_texture(const char* key) {
    return rbe_string_hash_map_has(texturesMap, key);
}

// --- Font --- //
Font* rbe_asset_manager_load_font(const char* fileName, const char* key, int size) {
    RBE_ASSERT_FMT(!rbe_asset_manager_has_font(key), "Font key '%s' already exists!", key);
    Font* font = font_create_font(fileName, size);
    RBE_ASSERT_FMT(font != NULL, "Failed to load font! file_name: '%s', key: '%s', size: '%d'", fileName, key, size);
    rbe_string_hash_map_add(fontMap, key, font, sizeof(Font));
    RBE_MEM_FREE(font);
    font = (Font*) rbe_string_hash_map_get(fontMap, key);
    return font;
}

Font* rbe_asset_manager_get_font(const char* key) {
    return (Font*) rbe_string_hash_map_get(fontMap, key);
}

bool rbe_asset_manager_has_font(const char* key) {
    return rbe_string_hash_map_has(fontMap, key);
}

// --- Audio Source --- //
RBEAudioSource* rbe_asset_manager_load_audio_source_wav(const char* fileName, const char* key) {
    RBE_ASSERT(audioSourceMap != NULL);
    RBE_ASSERT_FMT(!rbe_string_hash_map_has(audioSourceMap, fileName), "Already loaded audio source at file path '%'s!  Has key '%s'.", fileName, key);
    int32_t sampleCount;
    int32_t channels;
    int32_t sampleRate;
    void* samples = NULL;
    if (!rbe_audio_load_wav_data_from_file(fileName, &sampleCount, &channels, &sampleRate, &samples)) {
        rbe_logger_error("Failed to load audio wav file at '%s'", fileName);
        return NULL;
    }
    RBEAudioSource* newAudioSource = (RBEAudioSource*) RBE_MEM_ALLOCATE_SIZE(sizeof(RBEAudioSource*) + (sampleCount * sizeof(int16_t*)));
    newAudioSource->file_path = fileName;
    newAudioSource->sample_count = sampleCount;
    newAudioSource->channels = channels;
    newAudioSource->sample_rate = sampleRate;
    newAudioSource->samples = samples;
    rbe_string_hash_map_add(audioSourceMap, key, newAudioSource, sizeof(RBEAudioSource));
    RBE_MEM_FREE(newAudioSource);
    newAudioSource = (RBEAudioSource*) rbe_string_hash_map_get(audioSourceMap, key);
    rbe_audio_print_audio_source(newAudioSource);
    return newAudioSource;
}

RBEAudioSource* rbe_asset_manager_get_audio_source(const char* key) {
    return (RBEAudioSource*) rbe_string_hash_map_get(audioSourceMap, key);
}

bool rbe_asset_manager_has_audio_source(const char* key) {
    return rbe_string_hash_map_has(audioSourceMap, key);
}
