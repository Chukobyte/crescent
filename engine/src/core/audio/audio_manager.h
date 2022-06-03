#pragma once

#include <stdbool.h>

bool rbe_audio_manager_init();
void rbe_audio_manager_finalize();
void rbe_audio_manager_play_sound(const char* filePath);
