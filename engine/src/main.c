#include <stdlib.h>

#include <SDL2/SDL_main.h>

#include "core/core.h"

int main(int argv, char** args) {
    if (!rbe_initialize(argv, args)) {
        return EXIT_FAILURE;
    }

    while (rbe_is_running()) {
        rbe_update();
    }

    rbe_shutdown();
    return EXIT_SUCCESS;
}
