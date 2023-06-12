#include "audio_manager.h"

#include <stdint.h>
#include <string.h>

#include <miniaudio/miniaudio.h>

#include "audio.h"
#include "../asset/asset_manager.h"
#include "../memory/se_mem.h"
#include "../thread/se_pthread.h"
#include "../utils/logger.h"

#define MAX_AUDIO_INSTANCES 32

void audio_data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count);

static ma_device* audio_device = NULL;
static pthread_mutex_t audio_mutex;

// An instance of an RBE audio source
typedef struct RBEAudioInstance {
    SEAudioSource* source;
    unsigned int id;
    bool is_playing;
    bool does_loop;
    double sample_position;
} RBEAudioInstance;

struct AudioInstances {
    RBEAudioInstance* instances[MAX_AUDIO_INSTANCES];
    size_t count;
} AudioInstances;

static struct AudioInstances* audio_instances = NULL;

// --- Audio Manager --- //
bool se_audio_manager_init(uint32_t waveSampleRate) {
    audio_instances = SE_MEM_ALLOCATE(struct AudioInstances);
    pthread_mutex_init(&audio_mutex, NULL);
    // Device
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;
    config.playback.format = ma_format_s16;
    config.playback.channels = 2;
    config.capture.pDeviceID = NULL;
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = waveSampleRate;
    config.dataCallback = audio_data_callback;
    config.pUserData = NULL;
    audio_device = SE_MEM_ALLOCATE(ma_device);
    if (ma_device_init(NULL, &config, audio_device) != MA_SUCCESS) {
        se_logger_error("Failed to initialize miniaudio device!");
        return false;
    }

    if (ma_device_start(audio_device) != MA_SUCCESS) {
        se_logger_error("Failed to start audio device!");
        return false;
    }

    return true;
}

void se_audio_manager_finalize() {
    ma_device_uninit(audio_device);
    SE_MEM_FREE(audio_device);
    audio_device = NULL;

    SE_MEM_FREE(audio_instances); // TODO: Properly free up all instances
    audio_instances = NULL;

    pthread_mutex_destroy(&audio_mutex);
}

void se_audio_manager_play_sound(const char* filePath, bool loops) {
    if (!se_asset_manager_has_audio_source(filePath)) {
        se_logger_error("Doesn't have audio source loaded at path '%s' loaded!  Aborting...", filePath);
        return;
    } else if (audio_instances->count >= MAX_AUDIO_INSTANCES) {
        se_logger_warn("Reached max audio instances of '%d', not playing sound!", MAX_AUDIO_INSTANCES);
        return;
    }

    pthread_mutex_lock(&audio_mutex);
    // Create audio instance and add to instances array
    static unsigned int audioInstanceId = 0;  // TODO: temp id for now in case we need to grab a hold of an audio instance for roll back later...
    RBEAudioInstance* audioInstance = SE_MEM_ALLOCATE(RBEAudioInstance);
    audioInstance->source = se_asset_manager_get_audio_source(filePath);
    audioInstance->id = audioInstanceId++;
    audioInstance->does_loop = loops;
    audioInstance->sample_position = 0.0f;
    audioInstance->is_playing = true; // Sets sound instance to be played

    audio_instances->instances[audio_instances->count++] = audioInstance;
    se_logger_debug("Added audio instance from file path '%s' to play!", filePath);
    pthread_mutex_unlock(&audio_mutex);
}

void se_audio_manager_stop_sound(const char* filePath) {
    pthread_mutex_lock(&audio_mutex);
    for (size_t i = 0; i < audio_instances->count; i++) {
        RBEAudioInstance* audioInst = audio_instances->instances[i];
        if (strcmp(audioInst->source->file_path, filePath) == 0) {
            audioInst->is_playing = false;
            break;
        }
    }
    pthread_mutex_unlock(&audio_mutex);
}

// --- Mini Audio Callback --- //
void audio_data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count) {
    if (audio_instances->count <= 0) {
        return;
    }

    pthread_mutex_lock(&audio_mutex);
    memset(output, 0, frame_count * device->playback.channels * ma_get_bytes_per_sample(device->playback.format));
    size_t removedInstances = 0;
    for (size_t i = 0; i < audio_instances->count; i++) {
        RBEAudioInstance* audioInst = audio_instances->instances[i];
        if (!audioInst->is_playing) {
            audio_instances->instances[i] = NULL;
            removedInstances++;
            continue;
        }

        const int32_t channels = audioInst->source->channels;
        const double pitch = audioInst->source->pitch;
        int16_t* sampleOut = (int16_t*) output;
        int16_t* samples = (int16_t*) audioInst->source->samples;
        uint64_t samplesToWrite = (uint64_t) frame_count;

        // Write to output
        for (uint64_t writeSample = 0; writeSample < samplesToWrite; writeSample++) {
            double startSamplePosition = audioInst->sample_position;

            double targetSamplePosition = startSamplePosition + (double) channels * pitch;
            if (targetSamplePosition >= audioInst->source->sample_count) {
                targetSamplePosition -= (double) audioInst->source->sample_count;
            }

            uint64_t leftId = (uint64_t) startSamplePosition;
            if (channels > 1) {
                leftId &= ~((uint64_t)(0x01));
            }
            const uint64_t rightId = leftId + (uint64_t) (channels - 1);
            const int16_t startLeftSample = samples[leftId + channels];
            const int16_t startRightSample = samples[rightId + channels];

            const int16_t leftSample = (int16_t) (startLeftSample / channels);
            const int16_t rightSample = (int16_t) (startRightSample / channels);

            *sampleOut++ += leftSample;  // Left
            *sampleOut++ += rightSample; // Right

            // Possibly need fixed sampling instead
            audioInst->sample_position = targetSamplePosition;

            const bool isAtEnd = audioInst->sample_position >= audioInst->source->sample_count - channels - 1;
            if (isAtEnd) {
                audioInst->sample_position = 0;
                if (!audioInst->does_loop) {
                    se_logger_debug("Audio instance with id '%u' is queued for deletion!", audioInst->id);
                    audioInst->is_playing = false;
                    audio_instances->instances[i] = NULL;
                    removedInstances++;
                    SE_MEM_FREE(audioInst);
                    break;
                }
            }
        }
    }

    // Reshuffle array and update count if data sources have been removed
    if (removedInstances > 0) {
        for (size_t i = 0; i < audio_instances->count; i++) {
            if (audio_instances->instances[i] == NULL && i + 1 < audio_instances->count) {
                audio_instances->instances[i] = audio_instances->instances[i + 1];
                audio_instances->instances[i + 1] = NULL;
            }
        }
        audio_instances->count -= removedInstances;
    }
    pthread_mutex_unlock(&audio_mutex);
}
