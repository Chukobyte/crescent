#include "audio_manager.h"
#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio/miniaudio.h>
#include <stdint.h>

#include "audio.h"
#include "../asset_manager.h"
#include "../memory/rbe_mem.h"
#include "../thread/rbe_pthread.h"
#include "../utils/logger.h"
#include "../utils/rbe_file_system_utils.h"

#define MAX_AUDIO_INSTANCES 32

//typedef size_t wu;
//typedef uint64_t wu;

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

static ma_device* audioDevice = NULL;
static pthread_mutex_t* audioMutex = NULL;

// An instance of an RBE audio source
typedef struct RBEAudioInstance {
    RBEAudioSource* source;
    unsigned int id;
    bool isPlaying;
    bool doesLoop;
    double samplePosition;
} RBEAudioInstance;

struct AudioInstances {
    RBEAudioInstance* instances[MAX_AUDIO_INSTANCES];
    size_t count;
} AudioInstances;

static struct AudioInstances* audioInstances = NULL;

// --- Audio Manager --- //
bool rbe_audio_manager_init() {
    audioInstances = RBE_MEM_ALLOCATE(struct AudioInstances);
    pthread_mutex_init(audioMutex, NULL);
    // Device
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;
    config.playback.format = ma_format_s16;
    config.playback.channels = 2;
    config.capture.pDeviceID = NULL;
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = 44100;
    config.dataCallback = audio_data_callback;
    config.pUserData = NULL;
    audioDevice = RBE_MEM_ALLOCATE(ma_device);
    if (ma_device_init(NULL, &config, audioDevice) != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio device!");
        return false;
    }

    if (ma_device_start(audioDevice) != MA_SUCCESS) {
        rbe_logger_error("Failed to start audio device!");
        return false;
    }

    return true;
}

void rbe_audio_manager_finalize() {
    ma_device_uninit(audioDevice);
    RBE_MEM_FREE(audioDevice);
    audioDevice = NULL;

    RBE_MEM_FREE(audioInstances); // TODO: Properly free up all instances
    audioInstances = NULL;

    pthread_mutex_destroy(audioMutex);
}

void rbe_audio_manager_play_sound(const char* filePath, bool loops) {
    // Temp asset creation
    if (!rbe_asset_manager_has_audio_source(filePath)) {
        rbe_logger_error("Doesn't have audio source loaded at path '%s' loaded!  Aborting...", filePath);
        return;
    } else if (audioInstances->count >= MAX_AUDIO_INSTANCES) {
        rbe_logger_warn("Reached max audio instances of '%d', not playing sound!", MAX_AUDIO_INSTANCES);
        return;
    }

    // Create audio instance and add to instances array
    static unsigned int audioInstanceId = 0;  // TODO: temp id for now in case we need to grab a hold of an audio instance for roll back later...
    RBEAudioInstance* audioInstance = RBE_MEM_ALLOCATE(RBEAudioInstance);
    audioInstance->source = rbe_asset_manager_get_audio_source(filePath);
    audioInstance->id = audioInstanceId++;
    audioInstance->doesLoop = loops;
    audioInstance->samplePosition = 0.0f;
    audioInstance->isPlaying = true; // Sets sound instance to be played

    audioInstances->instances[audioInstances->count++] = audioInstance;
    rbe_logger_debug("Added audio instance from file path '%s' to play!", filePath);
}

// --- Mini Audio Callback --- //
void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    if (audioInstances->count <= 0) {
        return;
    }

    pthread_mutex_lock(audioMutex);
    memset(pOutput, 0, frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format));
    size_t removedInstances = 0;
    for (size_t i = 0; i < audioInstances->count; i++) {
        RBEAudioInstance* audioInst = audioInstances->instances[i];
        if (!audioInst->isPlaying) {
            audioInstances->instances[i] = NULL;
            removedInstances++;
            continue;
        }

        const int32_t channels = audioInst->source->channels;
        int16_t* sampleOut = (int16_t*) pOutput;
        int16_t* samples = (int16_t*) audioInst->source->samples;
        uint64_t samplesToWrite = (uint64_t) frameCount;

        // Write to output
//        rbe_logger_debug("Writing to output with instance id = %d", audioInst->id);
        for (uint64_t writeSample = 0; writeSample < samplesToWrite; writeSample++) {
            double startSamplePosition = audioInst->samplePosition;

            double targetSamplePosition = startSamplePosition + (double) channels * 1.1f; // TODO: Not sure why '1.1f'
            if (targetSamplePosition >= audioInst->source->sample_count) {
                targetSamplePosition -= (double) audioInst->source->sample_count;
            }
//            rbe_logger_debug("Write sample '%u' of '%u'.", writeSample, samplesToWrite);
//            rbe_logger_debug("Start Sample position = %f, target sample position = %f", startSamplePosition, targetSamplePosition);

            int16_t startLeftSample;
            int16_t startRightSample;
            {
                uint64_t leftId = (uint64_t) startSamplePosition;
                if (channels > 1) {
                    leftId &= ~((uint64_t)(0x01));
                }
                uint64_t rightId = leftId + (uint64_t) (channels - 1);

                int16_t firstLeftSample = samples[leftId];
                int16_t firstRightSample = samples[rightId];
                int16_t secondLeftSample = samples[leftId + channels];
                int16_t secondRightSample = samples[rightId + channels];

                startLeftSample = (int16_t) (firstLeftSample + secondLeftSample - firstLeftSample);
                startRightSample = (int16_t) (firstRightSample + secondRightSample - firstRightSample);
            }

            int16_t leftSample = (int16_t) (startLeftSample / channels);
            int16_t rightSample = (int16_t) (startRightSample / channels);

            *sampleOut++ += leftSample;  // Left
            *sampleOut++ += rightSample; // Right

            // Possibly need fixed sampling instead
            audioInst->samplePosition = targetSamplePosition;

            const bool isAtEnd = audioInst->samplePosition >= audioInst->source->sample_count - channels - 1;
            if (isAtEnd) {
                rbe_logger_debug("At the end!");
                audioInst->samplePosition = 0;
                if (!audioInst->doesLoop) {
                    audioInst->isPlaying = false;
                    audioInstances->instances[i] = NULL;
                    removedInstances++;
                    break;
                }
            }
        }
    }

    // Reshuffle array and update count if data sources have been removed
    if (removedInstances > 0) {
        rbe_logger_debug("Destroying '%d' audio instances!", removedInstances);
        for (size_t i = 0; i < audioInstances->count; i++) {
            if (audioInstances->instances[i] == NULL && i + 1 < audioInstances->count) {
                audioInstances->instances[i] = audioInstances->instances[i + 1];
                audioInstances->instances[i + 1] = NULL;
            }
        }
        audioInstances->count -= removedInstances;
        rbe_logger_debug("Destroying finished!");
    }
    pthread_mutex_unlock(audioMutex);
}

// --- RBE Audio --- //
bool rbe_audio_load_wav_data_from_file(const char* file_path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples) {
    size_t len = 0;
    char* file_data = rbe_fs_read_file_contents(file_path, &len);
    rbe_logger_debug("file '%s' size '%u' bytes", file_path, len);

    uint64_t totalPcmFrameCount = 0;
    *samples =  drwav_open_memory_and_read_pcm_frames_s16(file_data, len, (uint32_t*)channels, (uint32_t*)sample_rate, &totalPcmFrameCount, NULL);
//    RBE_MEM_FREE(file_data);
    drwav_free(file_data, NULL);

    if (!*samples) {
        *samples = NULL;
        rbe_logger_error("Could not load .wav file: %s", file_path);
        return false;
    }

    *sample_count = (int32_t) totalPcmFrameCount * *channels;

    return true;
}
