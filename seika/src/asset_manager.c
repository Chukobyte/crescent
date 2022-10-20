#include "asset_manager.h"

#include "data_structures/se_hash_map_string.h"
#include "rendering/texture.h"
#include "rendering/font.h"
#include "audio/audio.h"
#include "audio/audio_manager.h"
#include "memory/se_mem.h"
#include "utils/se_assert.h"

#define INITIAL_ASSET_HASHMAP_SIZE 8

SEStringHashMap* texturesMap = NULL;
SEStringHashMap* fontMap = NULL;
SEStringHashMap* audioSourceMap = NULL;

void se_asset_manager_initialize() {
    texturesMap = se_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
    fontMap = se_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
    audioSourceMap = se_string_hash_map_create(INITIAL_ASSET_HASHMAP_SIZE);
}

void se_asset_manager_finalize() {
    se_string_hash_map_destroy(texturesMap);
    se_string_hash_map_destroy(fontMap);
    se_string_hash_map_destroy(audioSourceMap);
}

// --- Texture --- //
Texture* se_asset_manager_load_texture(const char* fileName, const char* key) {
    SE_ASSERT(texturesMap != NULL);
    SE_ASSERT_FMT(!se_string_hash_map_has(texturesMap, fileName), "Already loaded texture at file path '%'s!  Has key '%s'.", fileName, key);
    Texture* texture = se_texture_create_texture(fileName);
    se_string_hash_map_add(texturesMap, key, texture, sizeof(Texture));
    SE_MEM_FREE(texture);
    texture = (Texture*) se_string_hash_map_get(texturesMap, key);
    return texture;
}

Texture* se_asset_manager_get_texture(const char* key) {
    return (Texture*) se_string_hash_map_get(texturesMap, key);
}

bool se_asset_manager_has_texture(const char* key) {
    return se_string_hash_map_has(texturesMap, key);
}

// --- Font --- //
Font* se_asset_manager_load_font(const char* fileName, const char* key, int size) {
    SE_ASSERT_FMT(!se_asset_manager_has_font(key), "Font key '%s' already exists!", key);
    Font* font = font_create_font(fileName, size);
    SE_ASSERT_FMT(font != NULL, "Failed to load font! file_name: '%s', key: '%s', size: '%d'", fileName, key, size);
    se_string_hash_map_add(fontMap, key, font, sizeof(Font));
    SE_MEM_FREE(font);
    font = (Font*) se_string_hash_map_get(fontMap, key);
    return font;
}

Font* se_asset_manager_get_font(const char* key) {
    return (Font*) se_string_hash_map_get(fontMap, key);
}

bool se_asset_manager_has_font(const char* key) {
    return se_string_hash_map_has(fontMap, key);
}

// --- Audio Source --- //
SEAudioSource* se_asset_manager_load_audio_source_wav(const char* fileName, const char* key) {
    SE_ASSERT(audioSourceMap != NULL);
    SE_ASSERT_FMT(!se_string_hash_map_has(audioSourceMap, fileName), "Already loaded audio source at file path '%'s!  Has key '%s'.", fileName, key);
    int32_t sampleCount;
    int32_t channels;
    int32_t sampleRate;
    void* samples = NULL;
    if (!se_audio_load_wav_data_from_file(fileName, &sampleCount, &channels, &sampleRate, &samples)) {
        se_logger_error("Failed to load audio wav file at '%s'", fileName);
        return NULL;
    }
    SEAudioSource* newAudioSource = (SEAudioSource*) SE_MEM_ALLOCATE_SIZE(sizeof(SEAudioSource*) + (sampleCount * sizeof(int16_t*)));
    newAudioSource->file_path = fileName;
    newAudioSource->sample_count = sampleCount;
    newAudioSource->channels = channels;
    newAudioSource->sample_rate = sampleRate;
    newAudioSource->samples = samples;
    se_string_hash_map_add(audioSourceMap, key, newAudioSource, sizeof(SEAudioSource));
    SE_MEM_FREE(newAudioSource);
    newAudioSource = (SEAudioSource*) se_string_hash_map_get(audioSourceMap, key);
    se_audio_print_audio_source(newAudioSource);
    return newAudioSource;
}

SEAudioSource* se_asset_manager_get_audio_source(const char* key) {
    return (SEAudioSource*) se_string_hash_map_get(audioSourceMap, key);
}

bool se_asset_manager_has_audio_source(const char* key) {
    return se_string_hash_map_has(audioSourceMap, key);
}
