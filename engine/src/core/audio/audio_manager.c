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

void audio_data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count);

static ma_device* audio_device = NULL;
static pthread_mutex_t audio_mutex;

// An instance of an RBE audio source
typedef struct RBEAudioInstance {
    RBEAudioSource* source;
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
bool rbe_audio_manager_init() {
    audio_instances = RBE_MEM_ALLOCATE(struct AudioInstances);
    pthread_mutex_init(&audio_mutex, NULL);
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
    audio_device = RBE_MEM_ALLOCATE(ma_device);
    if (ma_device_init(NULL, &config, audio_device) != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio device!");
        return false;
    }

    if (ma_device_start(audio_device) != MA_SUCCESS) {
        rbe_logger_error("Failed to start audio device!");
        return false;
    }

    return true;
}

void rbe_audio_manager_finalize() {
    ma_device_uninit(audio_device);
    RBE_MEM_FREE(audio_device);
    audio_device = NULL;

    RBE_MEM_FREE(audio_instances); // TODO: Properly free up all instances
    audio_instances = NULL;

    pthread_mutex_destroy(&audio_mutex);
}

void rbe_audio_manager_play_sound(const char* filePath, bool loops) {
    // Temp asset creation
    if (!rbe_asset_manager_has_audio_source(filePath)) {
        rbe_logger_error("Doesn't have audio source loaded at path '%s' loaded!  Aborting...", filePath);
        return;
    } else if (audio_instances->count >= MAX_AUDIO_INSTANCES) {
        rbe_logger_warn("Reached max audio instances of '%d', not playing sound!", MAX_AUDIO_INSTANCES);
        return;
    }

    // Create audio instance and add to instances array
    static unsigned int audioInstanceId = 0;  // TODO: temp id for now in case we need to grab a hold of an audio instance for roll back later...
    RBEAudioInstance* audioInstance = RBE_MEM_ALLOCATE(RBEAudioInstance);
    audioInstance->source = rbe_asset_manager_get_audio_source(filePath);
    audioInstance->id = audioInstanceId++;
    audioInstance->does_loop = loops;
    audioInstance->sample_position = 0.0f;
    audioInstance->is_playing = true; // Sets sound instance to be played

    audio_instances->instances[audio_instances->count++] = audioInstance;
    rbe_logger_debug("Added audio instance from file path '%s' to play!", filePath);
}

void rbe_audio_manager_stop_sound(const char* filePath) {
    for (size_t i = 0; i < audio_instances->count; i++) {
        RBEAudioInstance* audioInst = audio_instances->instances[i];
        if (strcmp(audioInst->source->file_path, filePath) == 0) {
            audioInst->is_playing = false;
            break;
        }
    }
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
        int16_t* sampleOut = (int16_t*) output;
        int16_t* samples = (int16_t*) audioInst->source->samples;
        uint64_t samplesToWrite = (uint64_t) frame_count;

        // Write to output
        for (uint64_t writeSample = 0; writeSample < samplesToWrite; writeSample++) {
            double startSamplePosition = audioInst->sample_position;

            double targetSamplePosition = startSamplePosition + (double) channels * 1.1f; // TODO: Not sure why '1.1f'
            if (targetSamplePosition >= audioInst->source->sample_count) {
                targetSamplePosition -= (double) audioInst->source->sample_count;
            }

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
            audioInst->sample_position = targetSamplePosition;

            const bool isAtEnd = audioInst->sample_position >= audioInst->source->sample_count - channels - 1;
            if (isAtEnd) {
                audioInst->sample_position = 0;
                if (!audioInst->does_loop) {
                    rbe_logger_debug("Audio instance with id '%u' is queued for deletion!", audioInst->id);
                    audioInst->is_playing = false;
                    audio_instances->instances[i] = NULL;
                    removedInstances++;
                    RBE_MEM_FREE(audioInst);
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

// --- RBE Audio --- //
bool rbe_audio_load_wav_data_from_file(const char* file_path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples) {
    size_t len = 0;
    char* file_data = rbe_fs_read_file_contents(file_path, &len);
    rbe_logger_debug("file '%s' size '%u' bytes", file_path, len);

    uint64_t totalPcmFrameCount = 0;
    *samples =  drwav_open_memory_and_read_pcm_frames_s16(file_data, len, (uint32_t*)channels, (uint32_t*)sample_rate, &totalPcmFrameCount, NULL);
    RBE_MEM_FREE(file_data);

    if (!*samples) {
        *samples = NULL;
        rbe_logger_error("Could not load .wav file: %s", file_path);
        return false;
    }

    *sample_count = (int32_t) totalPcmFrameCount * *channels;

    return true;
}
