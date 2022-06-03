#pragma once

#include <glad/glad.h>

typedef struct Texture {
    GLuint id;
    unsigned char* data;
    GLsizei width;
    GLsizei height;
    int nrChannels;
    // format
    GLint internalFormat;
    GLint imageFormat;
    // configuration
    GLint wrapS;
    GLint wrapT;
    GLint filterMin;
    GLint filterMag;
    const char* fileName;
} Texture;

Texture* rbe_texture_create_texture(const char* filePath);
