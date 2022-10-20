#include "audio.h"

#include "../utils/logger.h"

void rbe_audio_print_audio_source(RBEAudioSource* audioSource) {
    rbe_logger_debug("audio source | channels = %d, sample rate = %d, sample count = %d, samples = %x",
                     audioSource->channels, audioSource->sample_rate, audioSource->sample_count, audioSource->samples);
}
