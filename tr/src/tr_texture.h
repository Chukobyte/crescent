#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <glad/glad.h>

typedef struct TRTexture {
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
    char* fileName;
} TRTexture;

TRTexture* tr_texture_create_texture(const char* filePath);
void tr_texture_delete(TRTexture* texture);

#ifdef __cplusplus
}
#endif
