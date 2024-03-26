#include "world.h"

// Struct is private now, may want to expose later...
typedef struct CreWorld {
    f32 timeDilation;
} CreWorld;

CreWorld globalWorld = { .timeDilation = 1.0f };

void cre_world_set_time_dilation(f32 timeDilation) {
    globalWorld.timeDilation = timeDilation;
}

f32 cre_world_get_time_dilation() {
    return globalWorld.timeDilation;
}
