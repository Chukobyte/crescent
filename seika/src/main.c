#include <stdlib.h>

#include <SDL2/SDL_main.h>

#include "seika.h"

int main(int argv, char** args) {
    sf_initialize("Test", 800, 600);
    while (sf_is_running()) {
        sf_process_inputs();
        sf_update();
        sf_render();
    }
    sf_shutdown();
    return EXIT_SUCCESS;
}
