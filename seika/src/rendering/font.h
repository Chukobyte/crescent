#pragma once

#include <stdbool.h>

#include <glad/glad.h>

#include "../math/se_math.h"

typedef struct SECharacter {
    GLuint textureId;
    SEVector2 size;
    SEVector2 bearing;
    unsigned int advance;
} SECharacter;

typedef struct SEFont {
    bool isValid;
    GLuint VAO;
    GLuint VBO;
    int size;
    SECharacter characters[128]; // First 128 of ASCII set
} SEFont;

SEFont* se_font_create_font(const char* fileName, int size, bool applyNearestNeighbor);
