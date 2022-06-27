#include "texture.h"

#include <memory.h>

#include <stb_image/stb_image.h>

#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"

static const struct Texture DEFAULT_TEXTURE_REF = {
    .id = 0,
    .data = NULL,
    .width = 0,
    .height = 0,
    .nrChannels = 0,
    .internalFormat = GL_RGBA8,
    .imageFormat = GL_RGBA,
    .wrapS = GL_CLAMP_TO_BORDER,
    .wrapT = GL_CLAMP_TO_BORDER,
    .filterMin = GL_NEAREST,
    .filterMag = GL_NEAREST
};

bool rbe_texture_is_texture_valid(Texture* texture);

Texture* rbe_texture_create_default_texture() {
    Texture* texture = RBE_MEM_ALLOCATE(Texture);
    texture->fileName = NULL;
    texture->internalFormat = DEFAULT_TEXTURE_REF.internalFormat;
    texture->imageFormat = DEFAULT_TEXTURE_REF.imageFormat;
    texture->wrapS = DEFAULT_TEXTURE_REF.wrapS;
    texture->wrapT = DEFAULT_TEXTURE_REF.wrapT;
    texture->filterMin = DEFAULT_TEXTURE_REF.filterMin;
    texture->filterMag = DEFAULT_TEXTURE_REF.filterMag;
    return texture;
}

void rbe_texture_generate(Texture* texture);

Texture* rbe_texture_create_texture(const char* filePath) {
    Texture* texture = rbe_texture_create_default_texture();
    texture->fileName = filePath;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* imageData = stbi_load(filePath, &texture->width, &texture->height, &texture->nrChannels, 0);
    RBE_ASSERT_FMT(imageData != NULL, "Failed to load texture image at file path '%s'", filePath);
    texture->data = (unsigned char*) RBE_MEM_ALLOCATE_SIZE(sizeof(&imageData));
    memcpy(texture->data, imageData, sizeof(&imageData));
    texture->data = imageData;

    rbe_texture_generate(texture);

    return texture;
}

Texture* rbe_texture_create_solid_colored_texture(GLsizei width, GLsizei height, GLuint colorValue) {
    Texture* texture = rbe_texture_create_default_texture();
    texture->nrChannels = 4;
    texture->width = width;
    texture->height = height;

    const GLsizei dataSize = width * height * 4;
    texture->data = (unsigned char*) RBE_MEM_ALLOCATE_SIZE(dataSize);
    for (GLsizei i = 0; i < dataSize; i++) {
        texture->data[i] = colorValue;
    }

    rbe_texture_generate(texture);

    return texture;
}

void rbe_texture_generate(Texture* texture) {
    // OpenGL Stuff
    if (texture->nrChannels == 1) {
        texture->imageFormat = GL_RED;
    } else if (texture->nrChannels == 3) {
        texture->imageFormat = GL_RGB;
    } else if (texture->nrChannels == 4) {
        texture->imageFormat = GL_RGBA;
    }

    // Create texture
    glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);
    glTextureStorage2D(texture->id,
                       1, // levels
                       texture->internalFormat,
                       texture->width,
                       texture->height);
    glTextureSubImage2D(texture->id,
                        0, // level
                        0, // offset x
                        0, // offset y
                        texture->width,
                        texture->height,
                        texture->imageFormat,
                        GL_UNSIGNED_BYTE,
                        texture->data);

    glGenerateTextureMipmap(texture->id);
    // Wrap and filter modes
    glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, texture->wrapS);
    glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, texture->wrapT);
    glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, texture->filterMin);
    glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, texture->filterMag);

    RBE_ASSERT_FMT(rbe_texture_is_texture_valid(texture), "Texture at file path '%s' is not valid!", texture->fileName);
}

bool rbe_texture_is_texture_valid(Texture* texture) {
    return texture != NULL;
}
