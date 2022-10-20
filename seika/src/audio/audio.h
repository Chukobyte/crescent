#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct SEAudioSource {
    const char* file_path;
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} SEAudioSource;

void se_audio_print_audio_source(SEAudioSource* audioSource);
