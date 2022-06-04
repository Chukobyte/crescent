#include "audio_manager.h"
#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio/miniaudio.h>

#include "../utils/logger.h"
#include "../memory/rbe_mem.h"
#include "../thread/rbe_thread_pool.h"

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
void audio_manager_thread_job(void* arg);

static ma_engine* audioEngine = NULL;
static ma_device* audioDevice = NULL;
static ma_resource_manager* resourceManager = NULL;
static ThreadPool* audioJobTP = NULL; // Can use a global job queue

// Temp datasource
static ma_resource_manager_data_source* resourceManagerDataSource = NULL;

bool rbe_audio_manager_init() {
    audioEngine = RBE_MEM_ALLOCATE(ma_engine);
    ma_result result = ma_engine_init(NULL, audioEngine);
    if (result != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio!");
        return false;
    }
    // Resource Manager
    resourceManager = RBE_MEM_ALLOCATE(ma_resource_manager);
    ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
    resourceManagerConfig.decodedFormat = ma_format_f32;
    resourceManagerConfig.decodedSampleRate = 48000;
    resourceManagerConfig.jobThreadCount = 0; // Managing with custom job system threads
    resourceManagerConfig.flags = MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
    ma_result resourceManagerResult = ma_resource_manager_init(&resourceManagerConfig, resourceManager);
    if (resourceManagerResult != MA_SUCCESS) {
        ma_device_uninit(audioDevice);
        rbe_logger_error("Error failed to initialize resource manager!");
        return false;
    }
    // Device
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.sampleRate = 48000;
    config.dataCallback = audio_data_callback;
    config.pUserData = resourceManager;
    audioDevice = RBE_MEM_ALLOCATE(ma_device);
    if (ma_device_init(NULL, &config, audioDevice) != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio device!");
        return false;
    }

    ma_device_start(audioDevice);

    // TODO: Temp until job stuff is figured out
    audioJobTP = tpool_create(1);
    tpool_add_work(audioJobTP, audio_manager_thread_job, NULL);

    return true;
}

void rbe_audio_manager_finalize() {
    ma_engine_uninit(audioEngine);
    RBE_MEM_FREE(audioEngine);
    audioEngine = NULL;

    ma_device_uninit(audioDevice);
    RBE_MEM_FREE(audioDevice);
    audioDevice = NULL;

    tpool_destroy(audioJobTP);
}

void rbe_audio_manager_process() {
    // TODO: Make non blocking
//    tpool_add_work(audioJobTP, audio_manager_thread_job, NULL);
//    tpool_wait(audioJobTP);
}

void rbe_audio_manager_play_sound(const char* filePath, bool loops) {
//    ma_engine_play_sound(audioEngine, filePath, NULL);

    // Temp Resource Manager
//    ma_resource_manager_data_source dataSource;
    ma_result result;
    if (resourceManagerDataSource == NULL) {
        resourceManagerDataSource = RBE_MEM_ALLOCATE(ma_resource_manager_data_source);
        result = ma_resource_manager_data_source_init(resourceManager, filePath, 0, NULL, resourceManagerDataSource);
        if (result != MA_SUCCESS) {
            rbe_logger_error("Failed to play sound at file path: '%s'", filePath);
            return;
        }
        if (loops) {
            ma_data_source_set_looping(resourceManagerDataSource, true);
        }
    }

    // ...

    // A ma_resource_manager_data_source object is compatible with the ma_data_source API. To read data, just call
    // the ma_data_source_read_pcm_frames() like you would with any normal data source.
    result = ma_data_source_read_pcm_frames(resourceManagerDataSource, NULL, 1000, NULL);
    if (result != MA_SUCCESS) {
        // Failed to read PCM frames.
        rbe_logger_error("Error playing sound!  result = '%d'", result);
    }

//    ma_resource_manager_data_source_uninit(&dataSource);
}

// --- Thread/Job --- //
void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    if (resourceManagerDataSource != NULL) {
        ma_result result = ma_data_source_read_pcm_frames(resourceManagerDataSource, pOutput, frameCount, NULL);
        if (result != MA_SUCCESS) {
            // Failed to read PCM frames.
            rbe_logger_debug("Closing sound data resource because of result '%d'", result);
            ma_resource_manager_data_source_uninit(resourceManagerDataSource);
            resourceManagerDataSource = NULL;
        }
    }
}

void audio_manager_thread_job(void* arg) {
    (void) arg;
    while (true) {
//        printf("Hey\n");
        ma_job job;
        ma_result result = ma_resource_manager_next_job(resourceManager, &job);
        if (result != MA_SUCCESS) {
            if (result == MA_NO_DATA_AVAILABLE) {
                // No jobs are available. Keep going. Will only get this if the resource manager was initialized
                // with MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING.
                continue;
            }
            if (result == MA_CANCELLED) {
                rbe_logger_debug("Audio manager thread ended!");
                // MA_JOB_TYPE_QUIT was posted. Exit.
                break;
            } else {
                rbe_logger_debug("Audio manager thread ended in error! result = '%d'", result);
                // Some other error occurred.
                break;
            }
        }
        rbe_logger_debug("Job process!");
        ma_job_process(&job);
    }
}
