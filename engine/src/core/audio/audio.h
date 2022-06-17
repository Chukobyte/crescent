#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct RBEAudioSource {
    const char* file_path;
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} RBEAudioSource;

typedef struct Audio {
    int DUMMY_MEMBER; // FIXME: remove this, this is only there because of: 'error C2016: C requires that a struct or union have at least one member'
} Audio;

void rbe_audio_print_audio_source(RBEAudioSource* audioSource);
