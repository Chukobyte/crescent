#include "audio_manager.h"
#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio/miniaudio.h>

#include "../utils/logger.h"
#include "../memory/rbe_mem.h"

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {}

static ma_engine* audioEngine = NULL;
static ma_device* audioDevice = NULL;
static ma_resource_manager* resourceManager = NULL;

bool rbe_audio_manager_init() {
    audioEngine = RBE_MEM_ALLOCATE(ma_engine);
    ma_result result = ma_engine_init(NULL, audioEngine);
    if (result != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio!");
        return false;
    }
//    // Resource Manager
//    resourceManager = RBE_MEM_ALLOCATE(ma_resource_manager);
//    ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
//    resourceManagerConfig.decodedFormat = ma_format_f32;
//    resourceManagerConfig.decodedSampleRate = 48000;
//    ma_result resourceManagerResult = ma_resource_manager_init(&resourceManagerConfig, resourceManager);
//    if (resourceManagerResult != MA_SUCCESS) {
//        ma_device_uninit(audioDevice);
//        rbe_logger_error("Error failed to initialize resource manager!");
//        return false;
//    }
//    // Device
//    ma_device_config config = ma_device_config_init(ma_device_type_playback);
//    config.playback.format = ma_format_f32;
//    config.sampleRate = 48000;
//    config.dataCallback = audio_data_callback;
////    config.pUserData = NULL; // TODO: Do
//    config.pUserData = resourceManager;
//    audioDevice = RBE_MEM_ALLOCATE(ma_device);
//    if (ma_device_init(NULL, &config, audioDevice) != MA_SUCCESS) {
//        rbe_logger_error("Failed to initialize miniaudio device!");
//        return false;
//    }
//
//    ma_device_start(audioDevice);

    return true;
}

void rbe_audio_manager_finalize() {
    ma_engine_uninit(audioEngine);
    RBE_MEM_FREE(audioEngine);
    audioEngine = NULL;
//    ma_device_uninit(audioDevice);
//    RBE_MEM_FREE(audioDevice);
//    audioDevice = NULL;
}

void rbe_audio_manager_play_sound(const char* filePath) {
    ma_engine_play_sound(audioEngine, filePath, NULL);

    // Temp Resource Manager
//    ma_resource_manager_data_source dataSource;
//    ma_result result = ma_resource_manager_data_source_init(resourceManager, filePath, 0, NULL, &dataSource);
//    if (result != MA_SUCCESS) {
//        rbe_logger_error("Failed to play sound at file path: '%s'", filePath);
//        return;
//    }

    // ...

    // A ma_resource_manager_data_source object is compatible with the ma_data_source API. To read data, just call
    // the ma_data_source_read_pcm_frames() like you would with any normal data source.
//    result = ma_data_source_read_pcm_frames(&dataSource, pDecodedData, frameCount, &framesRead);
//    if (result != MA_SUCCESS) {
//        // Failed to read PCM frames.
//    }
//
//    ma_resource_manager_data_source_uninit(&dataSource);
}
