#include "audio_manager.h"
#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio/miniaudio.h>

#include "../utils/logger.h"

static ma_engine audioEngine;

bool rbe_audio_manager_init() {
    ma_result result = ma_engine_init(NULL, &audioEngine);
    if (result != MA_SUCCESS) {
        rbe_logger_error("Failed to initialize miniaudio!");
        return false;
    }

    return true;
}

void rbe_audio_manager_finalize() {
    ma_engine_uninit(&audioEngine);
}

void rbe_audio_manager_play_sound(const char* filePath) {
    ma_engine_play_sound(&audioEngine, filePath, NULL);
}
