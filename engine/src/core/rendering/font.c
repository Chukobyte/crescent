#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "render_context.h"
#include "../memory/rbe_mem.h"
#include "../utils/logger.h"

Font* font_create_font(const char* fileName, int size) {
    FT_Face face;
    Font* font = RBE_MEM_ALLOCATE(Font);
    font->size = size;
    if (FT_New_Face(rbe_render_context_get()->freeTypeLibrary, fileName, 0, &face)) {
        rbe_logger_error("Freetype failed to load font!");
        font->isValid = false;
    } else {
        // Set size to load glyphs, width set to 0 to dynamically adjust
        FT_Set_Pixel_Sizes(face, 0, size);
        // Disable byte alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // Load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++) { // 'c++' ;-)
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                rbe_logger_error("Freetype failed to load Glyph!");
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
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
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
            Character character = {
                .textureId = textTexture,
                .size = { face->glyph->bitmap.width, face->glyph->bitmap.rows },
                .bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top },
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
    }
    FT_Done_Face(face);
    return font;
}
