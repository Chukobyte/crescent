#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SE_AUDIO_SOURCE_DEFAULT_WAV_SAMPLE_RATE 44100

typedef struct SEAudioSource {
    const char* file_path;
    double pitch;
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} SEAudioSource;

void se_audio_print_audio_source(SEAudioSource* audioSource);
void se_audio_set_wav_sample_rate(uint32_t wavSampleRate);
uint32_t se_audio_get_wav_sample_rate();
SEAudioSource* se_audio_load_audio_source_wav(const char* fileName);
