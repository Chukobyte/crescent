#include "world.h"

// Struct is private now, may want to expose later...
typedef struct CreWorld {
    f32 timeDilation;
    f32 variableDeltaTime; // frame's variable delta time
} CreWorld;

CreWorld globalWorld = { .timeDilation = 1.0f, .variableDeltaTime = 0.0f };

void cre_world_set_time_dilation(f32 timeDilation) {
    globalWorld.timeDilation = timeDilation;
}

f32 cre_world_get_time_dilation() {
    return globalWorld.timeDilation;
}

void cre_world_set_frame_delta_time(f32 frameDeltaTime) {
    globalWorld.variableDeltaTime = frameDeltaTime;
}

f32 cre_world_get_frame_delta_time() {
    return globalWorld.variableDeltaTime;
}
