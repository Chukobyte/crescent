#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool rbe_audio_manager_init();
void rbe_audio_manager_finalize();
// Process current jobs for resource manager
void rbe_audio_manager_process();
void rbe_audio_manager_play_sound(const char* filePath, bool loops);

#ifdef __cplusplus
}
#endif
