#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "render_context.h"
#include "../asset/asset_file_loader.h"
#include "../memory/se_mem.h"
#include "../utils/logger.h"

bool generate_new_font_face(const char* fileName, FT_Face* face);

SEFont* se_font_create_font(const char* fileName, int size) {
    FT_Face face;
    SEFont* font = SE_MEM_ALLOCATE(SEFont);
    font->size = size;

    // Failed to create font, exit out early
    if (!generate_new_font_face(fileName, &face)) {
        se_logger_error("Freetype failed to load font!");
        font->isValid = false;
        FT_Done_Face(face);
        return font;
    }

    // Set size to load glyphs, width set to 0 to dynamically adjust
    FT_Set_Pixel_Sizes(face, 0, size);
    // Disable byte alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++) { // 'c++' ;-)
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            se_logger_error("Freetype failed to load glyph '%u!", c);
            continue;
        }
        // Generate texture
        GLuint textTexture;
        glGenTextures(1, &textTexture);
        glBindTexture(GL_TEXTURE_2D, textTexture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            (GLsizei) face->glyph->bitmap.width,
            (GLsizei) face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Texture wrap and filter options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Create character struct
        SECharacter character = {
            .textureId = textTexture,
            .size = { (float) face->glyph->bitmap.width, (float) face->glyph->bitmap.rows },
            .bearing = { (float) face->glyph->bitmap_left, (float) face->glyph->bitmap_top },
            .advance = (GLuint) face->glyph->advance.x
        };
        font->characters[c] = character;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // configure VAO & VBO texture quads
    glGenVertexArrays(1, &font->VAO);
    glGenBuffers(1, &font->VBO);
    glBindVertexArray(font->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    font->isValid = true;

    FT_Done_Face(face);
    return font;
}

bool generate_new_font_face(const char* fileName, FT_Face* face) {
    if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_ARCHIVE) {
        SEArchiveFileAsset fileAsset = sf_asset_file_loader_get_asset(fileName);
        if (sf_asset_file_loader_is_asset_valid(&fileAsset)) {
            if (FT_New_Memory_Face(se_render_context_get()->freeTypeLibrary, (unsigned char*) fileAsset.buffer, (FT_Long) fileAsset.bufferSize, 0, face)) {
                // Failed to create new face
                return false;
            }
        } else {
            // Failed to load asset from archive
            return false;
        }
    } else if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_DISK) {
        if (FT_New_Face(se_render_context_get()->freeTypeLibrary, fileName, 0, face)) {
            // Failed to create new face
            return false;
        }
    }
    return true;
}
