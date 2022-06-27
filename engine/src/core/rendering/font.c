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
            glCreateTextures(GL_TEXTURE_2D, 1, &textTexture);
            glTextureStorage2D(textTexture,
                               0, // level
                               GL_RED,
                               (GLsizei) face->glyph->bitmap.width,
                               (GLsizei) face->glyph->bitmap.rows);
            glTextureSubImage2D(textTexture,
                                0, // level
                                0, // offset x
                                0, // offset y
                                (GLsizei) face->glyph->bitmap.width,
                                (GLsizei) face->glyph->bitmap.rows,
                                GL_RED,
                                GL_UNSIGNED_BYTE,
                                face->glyph->bitmap.buffer);
            // Texture wrap and filter options
            glTextureParameteri(textTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(textTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(textTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Create character struct
            Character character = {
                .textureId = textTexture,
                .size = { (float) face->glyph->bitmap.width, (float) face->glyph->bitmap.rows },
                .bearing = { (float) face->glyph->bitmap_left, (float) face->glyph->bitmap_top },
                .advance = (GLuint) face->glyph->advance.x
            };
            font->characters[c] = character;
        }

        // configure VAO & VBO texture quads
        glCreateVertexArrays(1, &font->VAO);
        glCreateBuffers(1, &font->VBO);
        glNamedBufferData(font->VBO, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexArrayAttrib(font->VAO, 0);
        glVertexArrayAttribFormat(font->VAO, 0, 4, GL_FLOAT, GL_FALSE, 0); // TODO: if there is an issue it might be here
        glVertexArrayAttribBinding(font->VAO, 0, 0);

        font->isValid = true;
    }
    FT_Done_Face(face);
    return font;
}
