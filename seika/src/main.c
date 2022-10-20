#include <stdlib.h>

#include <SDL2/SDL_main.h>

#include "seika.h"

int main(int argv, char** args) {
    sf_initialize("Test", 800, 600);
    for (int i = 0; i < 100000; i++) {}
    sf_shutdown();
    return EXIT_SUCCESS;
}
