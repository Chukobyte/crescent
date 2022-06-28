#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct RBEAudioSource {
    const char* file_path;
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} RBEAudioSource;

void rbe_audio_print_audio_source(RBEAudioSource* audioSource);
