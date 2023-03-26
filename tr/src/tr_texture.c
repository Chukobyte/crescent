#include "tr_texture.h"

#include <string.h>
#include <memory.h>
#include <assert.h>

#include <stb_image/stb_image.h>

#include "tr_mem.h"

static const struct TRTexture DEFAULT_TEXTURE_REF = {
    .id = 0,
    .data = NULL,
    .width = 0,
    .height = 0,
    .nrChannels = 0,
    .internalFormat = GL_RGBA,
    .imageFormat = GL_RGBA,
    .wrapS = GL_CLAMP_TO_BORDER,
    .wrapT = GL_CLAMP_TO_BORDER,
    .fileName = NULL
};

bool tr_texture_is_texture_valid(TRTexture* texture);

void tr_texture_generate(TRTexture* texture);

TRTexture* tr_texture_create_texture(const char* filePath) {
    TRTexture* texture = TR_MEM_ALLOCATE(TRTexture);
    texture->fileName = strdup(filePath);
    texture->internalFormat = DEFAULT_TEXTURE_REF.internalFormat;
    texture->wrapS = DEFAULT_TEXTURE_REF.wrapS;
    texture->wrapT = DEFAULT_TEXTURE_REF.wrapT;
    // Load image data
    texture->data = stbi_load(filePath, &texture->width, &texture->height, &texture->nrChannels, 0);
    assert(texture->data != NULL && "Failed to load texture image");
    tr_texture_generate(texture);
    return texture;
}

void tr_texture_generate(TRTexture* texture) {
    // OpenGL Stuff
    if (texture->nrChannels == 1) {
        texture->imageFormat = GL_RED;
    } else if (texture->nrChannels == 3) {
        texture->imageFormat = GL_RGB;
    } else if (texture->nrChannels == 4) {
        texture->imageFormat = GL_RGBA;
    }

    // Create texture
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, texture->width, texture->height, 0, texture->imageFormat, GL_UNSIGNED_BYTE, texture->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT);
    // Defaults to bilinear interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    assert(tr_texture_is_texture_valid(texture) && "Texture is not valid!");
}

bool tr_texture_is_texture_valid(TRTexture* texture) {
    return texture != NULL;
}

void tr_texture_delete(TRTexture* texture) {
    TR_MEM_FREE(texture->fileName);
    TR_MEM_FREE(texture);
}
