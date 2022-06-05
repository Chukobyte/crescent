#include "audio_manager.h"
#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio/miniaudio.h>
#include <stdint.h>

#include "audio.h"
#include "../asset_manager.h"
#include "../utils/logger.h"
#include "../memory/rbe_mem.h"
#include "../utils/rbe_file_system_utils.h"

#define MAX_AUDIO_INSTANCES 32

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

static ma_device* audioDevice = NULL;

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
    // Device
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;
    config.playback.format = ma_format_s16;
    config.playback.channels = 2;
    config.capture.pDeviceID = NULL;
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = 48000;
    config.dataCallback = audio_data_callback;
    config.pUserData = NULL;
    audioDevice = RBE_MEM_ALLOCATE(ma_device);
    if (ma_device_init(NULL, &config, audioDevice) != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio device!");
        return false;
    }

    ma_device_start(audioDevice);

    return true;
}

void rbe_audio_manager_finalize() {
    ma_device_uninit(audioDevice);
    RBE_MEM_FREE(audioDevice);
    audioDevice = NULL;

    RBE_MEM_FREE(audioInstances); // TODO: Properly free up all instances
    audioInstances = NULL;
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
    audioInstance->samplePosition = 0;
    audioInstance->isPlaying = true; // Sets sound instance to be played

    audioInstances->instances[audioInstances->count++] = audioInstance;
}

// --- Mini Audio Callback --- //
void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    size_t removedDataSources = 0;
    size_t removedInstances = 0;
    for (size_t i = 0; audioInstances->count; i++) {
        RBEAudioInstance* audioInst = audioInstances->instances[i];

        // ... do stuff

        const bool isAtEnd = audioInst->samplePosition >= audioInst->source->sample_count - audioInst->source->channels - 1;
        if (isAtEnd) {
            audioInst->samplePosition = 0;
            if (!audioInst->doesLoop) {
                audioInst->isPlaying = false;
                audioInstances->instances[i] = NULL;
                removedInstances++;
            }
        }
    }

    // Reshuffle array and update count if data sources have been removed
    if (removedInstances > 0) {
        for (size_t i = 0; i < audioInstances->count; i++) {
            if (audioInstances->instances[i] == NULL && i + 1 < audioInstances->count) {
                audioInstances->instances[i] = audioInstances->instances[i + 1];
                audioInstances->instances[i + 1] = NULL;
            }
        }
        audioInstances->count -= removedInstances;
    }
}

// --- RBE Audio --- //
bool rbe_audio_load_wav_data_from_file(const char* file_path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples) {
    size_t len = 0;
    char* file_data = rbe_fs_read_file_contents(file_path, &len);

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
