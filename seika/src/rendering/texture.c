#include "texture.h"

#include <memory.h>

#include <stb_image/stb_image.h>

#include "../memory/se_mem.h"
#include "../utils/se_assert.h"

static const struct Texture DEFAULT_TEXTURE_REF = {
    .id = 0,
    .data = NULL,
    .width = 0,
    .height = 0,
    .nrChannels = 0,
    .internalFormat = GL_RGBA,
    .imageFormat = GL_RGBA,
    .wrapS = GL_CLAMP_TO_BORDER,
    .wrapT = GL_CLAMP_TO_BORDER,
    .filterMin = GL_NEAREST,
    .filterMag = GL_NEAREST
};

bool se_texture_is_texture_valid(Texture* texture);

Texture* se_texture_create_default_texture() {
    Texture* texture = SE_MEM_ALLOCATE(Texture);
    texture->fileName = NULL;
    texture->internalFormat = DEFAULT_TEXTURE_REF.internalFormat;
    texture->imageFormat = DEFAULT_TEXTURE_REF.imageFormat;
    texture->wrapS = DEFAULT_TEXTURE_REF.wrapS;
    texture->wrapT = DEFAULT_TEXTURE_REF.wrapT;
    texture->filterMin = DEFAULT_TEXTURE_REF.filterMin;
    texture->filterMag = DEFAULT_TEXTURE_REF.filterMag;
    return texture;
}

void se_texture_generate(Texture* texture);

Texture* se_texture_create_texture(const char* filePath) {
    Texture* texture = se_texture_create_default_texture();
    texture->fileName = filePath;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* imageData = stbi_load(filePath, &texture->width, &texture->height, &texture->nrChannels, 0);
    SE_ASSERT_FMT(imageData != NULL, "Failed to load texture image at file path '%s'", filePath);
    const size_t imageDataSize = sizeof(Texture*);
    texture->data = (unsigned char*) SE_MEM_ALLOCATE_SIZE(imageDataSize);
    memcpy(texture->data, imageData, imageDataSize);
    texture->data = imageData;

    se_texture_generate(texture);

    stbi_image_free(imageData);
    return texture;
}

Texture* se_texture_create_solid_colored_texture(GLsizei width, GLsizei height, GLuint colorValue) {
    Texture* texture = se_texture_create_default_texture();
    texture->nrChannels = 4;
    texture->width = width;
    texture->height = height;

    const GLsizei dataSize = width * height * 4;
    texture->data = (unsigned char*) SE_MEM_ALLOCATE_SIZE(dataSize);
    for (GLsizei i = 0; i < dataSize; i++) {
        texture->data[i] = colorValue;
    }

    se_texture_generate(texture);

    return texture;
}

void se_texture_generate(Texture* texture) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filterMag);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    SE_ASSERT_FMT(se_texture_is_texture_valid(texture), "Texture at file path '%s' is not valid!", texture->fileName);
}

bool se_texture_is_texture_valid(Texture* texture) {
    return texture != NULL;
}
