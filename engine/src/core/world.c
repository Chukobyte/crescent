#include "world.h"

// Struct is private now, may want to expose later...
typedef struct CreWorld {
    float timeDilation;
} CreWorld;

CreWorld globalWorld = { .timeDilation = 1.0f };

void cre_world_set_time_dilation(float timeDilation) {
    globalWorld.timeDilation = timeDilation;
}

float cre_world_get_time_dilation() {
    return globalWorld.timeDilation;
}
