#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// --- Audio Manager --- //
bool se_audio_manager_init(uint32_t wavSampleRate);
void se_audio_manager_finalize();
// Process current jobs for resource manager
void se_audio_manager_play_sound(const char* filePath, bool loops);
void se_audio_manager_stop_sound(const char* filePath);

#ifdef __cplusplus
}
#endif
