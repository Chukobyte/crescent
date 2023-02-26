#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <glad/glad.h>

typedef struct SETexture {
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
    bool applyNearestNeighbor;
    char* fileName;
} SETexture;

SETexture* se_texture_create_texture(const char* filePath);
SETexture* se_texture_create_texture_ex(const char* filePath, GLint wrapS, GLint wrapT, bool applyNearestNeighbor);
SETexture* se_texture_create_solid_colored_texture(GLsizei width, GLsizei height, GLuint colorValue);
void se_texture_delete(SETexture* texture);
GLint se_texture_wrap_string_to_int(const char* wrap);

#ifdef __cplusplus
}
#endif
