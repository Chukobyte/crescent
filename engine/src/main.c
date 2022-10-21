#include <stdlib.h>

#include <SDL2/SDL_main.h>

#include "core/core.h"

int main(int argv, char** args) {
    if (!cre_initialize(argv, args)) {
        return EXIT_FAILURE;
    }

    while (cre_is_running()) {
        cre_update();
    }

    cre_shutdown();
    return EXIT_SUCCESS;
}
