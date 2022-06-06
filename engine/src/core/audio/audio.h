#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct RBEAudioSource {
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} RBEAudioSource;

typedef struct Audio {

} Audio;

void rbe_audio_print_audio_source(RBEAudioSource* audioSource);
