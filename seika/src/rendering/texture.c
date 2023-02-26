#include "texture.h"

#include <string.h>
#include <memory.h>

#include <stb_image/stb_image.h>

#include "../asset/asset_file_loader.h"
#include "../memory/se_mem.h"
#include "../utils/se_string_util.h"
#include "../utils/se_assert.h"

static const struct SETexture DEFAULT_TEXTURE_REF = {
    .id = 0,
    .data = NULL,
    .width = 0,
    .height = 0,
    .nrChannels = 0,
    .internalFormat = GL_RGBA,
    .imageFormat = GL_RGBA,
    .wrapS = GL_CLAMP_TO_BORDER,
    .wrapT = GL_CLAMP_TO_BORDER,
    .applyNearestNeighbor = true,
    .fileName = NULL
};

bool se_texture_is_texture_valid(SETexture* texture);

SETexture* se_texture_create_default_texture() {
    SETexture* texture = SE_MEM_ALLOCATE(SETexture);
    memcpy(texture, &DEFAULT_TEXTURE_REF, sizeof(SETexture));
    return texture;
}

void se_texture_generate(SETexture* texture);

SETexture* se_texture_create_texture(const char* filePath) {
    return se_texture_create_texture_ex(filePath, DEFAULT_TEXTURE_REF.wrapS, DEFAULT_TEXTURE_REF.wrapT, DEFAULT_TEXTURE_REF.applyNearestNeighbor);
}

SETexture* se_texture_create_texture_ex(const char* filePath, GLint wrapS, GLint wrapT, bool applyNearestNeighbor) {
    SETexture* texture = se_texture_create_default_texture();
    texture->fileName = se_strdup(filePath);
    texture->wrapS = wrapS;
    texture->wrapT = wrapT;
    texture->applyNearestNeighbor = applyNearestNeighbor;
    // Load image data
    SEAssetFileImageData* fileImageData = sf_asset_file_loader_load_image_data(filePath);
    SE_ASSERT_FMT(fileImageData != NULL, "Failed to load texture image at file path '%s'", filePath);
    const size_t imageDataSize = strlen((char*) fileImageData->data);
    texture->data = (unsigned char*) SE_MEM_ALLOCATE_SIZE(imageDataSize);
    memcpy(texture->data, fileImageData->data, imageDataSize);
    texture->data = fileImageData->data; // TODO: Fix
    texture->width = fileImageData->width;
    texture->height = fileImageData->height;
    texture->nrChannels = fileImageData->nrChannels;

    se_texture_generate(texture);

    sf_asset_file_loader_free_image_data(fileImageData);

    return texture;
}

SETexture* se_texture_create_solid_colored_texture(GLsizei width, GLsizei height, GLuint colorValue) {
    SETexture* texture = se_texture_create_default_texture();
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

void se_texture_generate(SETexture* texture) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    SE_ASSERT_FMT(se_texture_is_texture_valid(texture), "Texture at file path '%s' is not valid!", texture->fileName);
}

bool se_texture_is_texture_valid(SETexture* texture) {
    return texture != NULL;
}

void se_texture_delete(SETexture* texture) {
    SE_MEM_FREE(texture->fileName);
    SE_MEM_FREE(texture);
}

GLint se_texture_wrap_string_to_int(const char* wrap) {
    if (strcmp(wrap, "clamp_to_border") == 0) {
        return GL_CLAMP_TO_BORDER;
    } else if (strcmp(wrap, "repeat") == 0) {
        return GL_REPEAT;
    }
    return GL_CLAMP_TO_BORDER;
}

const char* se_texture_get_wrap_s_string(SETexture* texture) {
    return texture->wrapS == GL_REPEAT ? "repeat" : "clamp_to_border";
}

const char* se_texture_get_wrap_t_string(SETexture* texture) {
    return texture->wrapT == GL_REPEAT ? "repeat" : "clamp_to_border";
}
