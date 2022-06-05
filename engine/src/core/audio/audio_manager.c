#include "audio_manager.h"
#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio/miniaudio.h>
#include <stdint.h>

#include "../utils/logger.h"
#include "../memory/rbe_mem.h"
#include "../thread/rbe_thread_pool.h"
#include "../utils/rbe_file_system_utils.h"
#include "../data_structures/rbe_hash_map_string.h"

#define MAX_AUDIO_DATA_SOURCES 100

void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
bool audio_load_wav_data_from_file(const char* file_path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples);
//void audio_manager_thread_job(void* arg);

static ma_engine* audioEngine = NULL;
static ma_device* audioDevice = NULL;
static ma_resource_manager* resourceManager = NULL;
//static ThreadPool* audioJobTP = NULL; // Can use a global job queue
static RBEStringHashMap* rbeDataSources = NULL;

struct ResourceManagerDataSources {
    ma_resource_manager_data_source* data[MAX_AUDIO_DATA_SOURCES];
    size_t count;
} ResourceManagerDataSources;

static struct ResourceManagerDataSources* dataSources = NULL;

typedef struct RBEAudioSource {
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} RBEAudioSource;

bool rbe_audio_manager_init() {
    rbeDataSources = rbe_string_hash_map_create(32);
    audioEngine = RBE_MEM_ALLOCATE(ma_engine);
    ma_result result = ma_engine_init(NULL, audioEngine);
    if (result != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio!");
        return false;
    }
    // Data Sources
    dataSources = RBE_MEM_ALLOCATE(struct ResourceManagerDataSources);
    dataSources->count = 0;
    // Resource Manager
    resourceManager = RBE_MEM_ALLOCATE(ma_resource_manager);
    ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
    resourceManagerConfig.decodedFormat = ma_format_s16;
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
    config.playback.pDeviceID = NULL;
    config.playback.format = ma_format_s16;
    config.playback.channels = 2;
    config.capture.pDeviceID = NULL;
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = 48000;
    config.dataCallback = audio_data_callback;
    config.pUserData = resourceManager;
//    config.pUserData = NULL;
    audioDevice = RBE_MEM_ALLOCATE(ma_device);
    if (ma_device_init(NULL, &config, audioDevice) != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio device!");
        return false;
    }

    ma_device_start(audioDevice);

    // TODO: Temp until job stuff is figured out
//    audioJobTP = tpool_create(1);
//    tpool_add_work(audioJobTP, audio_manager_thread_job, NULL);

    // Temp

//    memset(dataSources, 0, sizeof(struct ResourceManagerDataSources));

    return true;
}

void rbe_audio_manager_finalize() {
    ma_engine_uninit(audioEngine);
    RBE_MEM_FREE(audioEngine);
    audioEngine = NULL;

    ma_device_uninit(audioDevice);
    RBE_MEM_FREE(audioDevice);
    audioDevice = NULL;

    RBE_MEM_FREE(dataSources); // TODO: Free up data in data sources...
    dataSources = NULL;

    rbe_string_hash_map_destroy(rbeDataSources);
    rbeDataSources = NULL;

//    tpool_destroy(audioJobTP);
}

void rbe_audio_manager_process() {
    // TODO: Make non blocking
//    tpool_add_work(audioJobTP, audio_manager_thread_job, NULL);
//    tpool_wait(audioJobTP);
}

void rbe_audio_manager_play_sound(const char* filePath, bool loops) {
    // Temp asset creation
    if (!rbe_string_hash_map_has(rbeDataSources, filePath)) {
        RBEAudioSource* newAudioSource = RBE_MEM_ALLOCATE(RBEAudioSource);
        if (!audio_load_wav_data_from_file(filePath, &newAudioSource->sample_count, &newAudioSource->channels, &newAudioSource->sample_rate, &newAudioSource->samples)) {
            rbe_logger_error("Failed to load audio wav file at '%s'", filePath);
            return;
        }
        rbe_string_hash_map_add(rbeDataSources, filePath, newAudioSource, sizeof(RBEAudioSource*));
    }

    ma_result result;
    ma_resource_manager_data_source* newDataSource = RBE_MEM_ALLOCATE(ma_resource_manager_data_source);
    result = ma_resource_manager_data_source_init(resourceManager, filePath, 0, NULL, newDataSource);
    if (result != MA_SUCCESS) {
        rbe_logger_error("Failed to play sound at file path: '%s'", filePath);
        return;
    }
    ma_data_source_set_looping(newDataSource, loops);
    dataSources->data[dataSources->count++] = newDataSource;
    const size_t newDataSourceIndex = dataSources->count - 1;
    rbe_logger_debug("New Data source at file path = '%s' created with index = %d", filePath, newDataSourceIndex);

    // A ma_resource_manager_data_source object is compatible with the ma_data_source API. To read data, just call
    // the ma_data_source_read_pcm_frames() like you would with any normal data source.
    result = ma_data_source_read_pcm_frames(dataSources->data[newDataSourceIndex], NULL, 1000, NULL);
    if (result != MA_SUCCESS) {
        // Failed to read PCM frames.
        rbe_logger_error("Error playing sound!  result = '%d'", result);
    }
}

// --- Thread/Job --- //
void audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    size_t removedDataSources = 0;
    for (size_t i = 0; i < dataSources->count; i++) {
        if (dataSources->data[i] == NULL) {
            rbe_logger_error("Trying to read null audio data with index = %d", i);
            continue;
        }
        ma_result result = ma_data_source_read_pcm_frames(dataSources->data[i], pOutput, frameCount, NULL);
        if (result != MA_SUCCESS) {
            // Failed to read PCM frames.
            if (result == MA_AT_END) {
                rbe_logger_debug("Sound data resource with index '%d' reached the end... data source count '%d'", i, dataSources->count - 1);
            } else {
                rbe_logger_error("Error in audio manager 'audio data callback' result '%d' with index '%d'", result, i);
            }
            ma_resource_manager_data_source_uninit(dataSources->data[i]);
            dataSources->data[i] = NULL;
            removedDataSources++;
        }
    }
    // Reshuffle array and update count if data sources have been removed
    if (removedDataSources > 0) {
        for (size_t i = 0; i < dataSources->count; i++) {
            if (dataSources->data[i] == NULL && i + 1 < dataSources->count) {
                dataSources->data[i] = dataSources->data[i + 1];
                dataSources->data[i + 1] = NULL;
            }
        }
        dataSources->count -= removedDataSources;
    }
}

bool audio_load_wav_data_from_file(const char* file_path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples) {
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

//void audio_manager_thread_job(void* arg) {
//    (void) arg;
//    while (true) {
//        ma_job job;
//        ma_result result = ma_resource_manager_next_job(resourceManager, &job);
//        if (result != MA_SUCCESS) {
//            if (result == MA_NO_DATA_AVAILABLE) {
//                // No jobs are available. Keep going. Will only get this if the resource manager was initialized
//                // with MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING.
//                continue;
//            }
//            if (result == MA_CANCELLED) {
//                rbe_logger_debug("Audio manager thread ended!");
//                // MA_JOB_TYPE_QUIT was posted. Exit.
//                break;
//            } else {
//                rbe_logger_debug("Audio manager thread ended in error! result = '%d'", result);
//                // Some other error occurred.
//                break;
//            }
//        }
//        rbe_logger_debug("Job process!");
//        ma_job_process(&job);
//    }
//}

