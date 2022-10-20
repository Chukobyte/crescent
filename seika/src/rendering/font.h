#pragma once

#include <stdbool.h>

#include <glad/glad.h>

#include "../math/rbe_math.h"

typedef struct Character {
    GLuint textureId;
    Vector2 size;
    Vector2 bearing;
    unsigned int advance;
} Character;

typedef struct Font {
    bool isValid;
    GLuint VAO;
    GLuint VBO;
    int size;
    Character characters[128]; // First 128 of ASCII set
} Font;

Font* font_create_font(const char* fileName, int size);
