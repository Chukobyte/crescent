#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// --- Audio Manager --- //
bool rbe_audio_manager_init();
void rbe_audio_manager_finalize();
// Process current jobs for resource manager
void rbe_audio_manager_play_sound(const char* filePath, bool loops);
// --- Audio --- //
bool rbe_audio_load_wav_data_from_file(const char* file_path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples);

#ifdef __cplusplus
}
#endif
