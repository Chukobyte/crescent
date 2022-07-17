#include "renderer.h"

#include <stdlib.h>

#include <cglm/cglm.h>

#include "render_context.h"
#include "shader.h"
#include "shader_source.h"
#include "../game_properties.h"
#include "../data_structures/rbe_static_array.h"
#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"

typedef struct TextureCoordinates {
    GLfloat sMin;
    GLfloat sMax;
    GLfloat tMin;
    GLfloat tMax;
} TextureCoordinates;

void sprite_renderer_initialize();
void sprite_renderer_finalize();
void sprite_renderer_draw_sprite(const Texture* texture, const Rect2* sourceRect, const Rect2* destRect, const Color *color, bool flipX, bool flipY, TransformModel2D* globalTransform);
void font_renderer_initialize();
void font_renderer_draw_text(const Font* font, const char* text, float x, float y, float scale, const Color* color);
void font_renderer_finalize();

TextureCoordinates renderer_get_texture_coordinates(const Texture* texture, const Rect2* drawSource, bool flipX, bool flipY);
void renderer_print_opengl_errors();

static GLuint spriteQuadVAO;
static GLuint spriteQuadVBO;

static Shader* spriteShader = NULL;
static Shader* fontShader = NULL;

// --- Renderer --- //
void rbe_renderer_initialize() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    rbe_render_context_initialize();
    sprite_renderer_initialize();
    font_renderer_initialize();
}

void rbe_renderer_finalize() {
    font_renderer_finalize();
    sprite_renderer_finalize();
    rbe_render_context_finalize();
}

// --- Sprite Batching --- //
typedef struct SpriteBatchItem {
    Texture* texture;
    Rect2 sourceRect;
    Rect2 destRect;
    Color color;
    bool flipX;
    bool flipY;
    TransformModel2D* globalTransform;
} SpriteBatchItem;

typedef struct FontBatchItem {
    Font* font;
    const char* text;
    float x;
    float y;
    float scale;
    Color color;
    Transform2DComponent* transform2DComponent;
} FontBatchItem;

RBE_STATIC_ARRAY_CREATE(SpriteBatchItem, 100, sprite_batch_items);
RBE_STATIC_ARRAY_CREATE(FontBatchItem, 100, font_batch_items);

void rbe_renderer_queue_sprite_draw_call(Texture* texture, Rect2 sourceRect, Rect2 destRect, Color color, bool flipX, bool flipY, TransformModel2D* globalTransform) {
    if (texture == NULL) {
        rbe_logger_error("NULL texture, not submitting draw call!");
        return;
    }
    SpriteBatchItem item = { .texture = texture, .sourceRect = sourceRect, .destRect = destRect, .color = color, .flipX = flipX, .flipY = flipY, .globalTransform = globalTransform };
    RBE_STATIC_ARRAY_ADD(sprite_batch_items, item);
}

void rbe_renderer_queue_font_draw_call(Font* font, const char* text, float x, float y, float scale, Color color) {
    FontBatchItem item = { .font = font, .text = text, .x = x, .y = y, .scale = scale, .color = color };
    RBE_STATIC_ARRAY_ADD(font_batch_items, item);
}

void rbe_renderer_flush_batches() {
    // Sprite
    for (size_t i = 0; i < sprite_batch_items_count; i++) {
        sprite_renderer_draw_sprite(
            sprite_batch_items[i].texture,
            &sprite_batch_items[i].sourceRect,
            &sprite_batch_items[i].destRect,
            &sprite_batch_items[i].color,
            sprite_batch_items[i].flipX,
            sprite_batch_items[i].flipY,
            sprite_batch_items[i].globalTransform
        );
    }
    RBE_STATIC_ARRAY_EMPTY(sprite_batch_items);
    // Fonts
    for (size_t i = 0; i < font_batch_items_count; i++) {
        font_renderer_draw_text(
            font_batch_items[i].font,
            font_batch_items[i].text,
            font_batch_items[i].x,
            font_batch_items[i].y,
            font_batch_items[i].scale,
            &font_batch_items[i].color
        );
    }
    RBE_STATIC_ARRAY_EMPTY(font_batch_items);
}

// --- Sprite Renderer --- //
void sprite_renderer_initialize() {
    GLfloat vertices[] = {
        //id          // positions       // texture coordinates // color
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    // Initialize render data
    glGenVertexArrays(1, &spriteQuadVAO);
    glGenBuffers(1, &spriteQuadVBO);

    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindVertexArray(spriteQuadVAO);
    // id attribute
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) NULL);
    glEnableVertexAttribArray(0);
    // position attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // color attribute
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // compile shaders
    spriteShader = shader_compile_new_shader(OPENGL_SHADER_SOURCE_VERTEX_SPRITE, OPENGL_SHADER_SOURCE_FRAGMENT_SPRITE);
    mat4 proj = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    RBEGameProperties* gameProperties = rbe_game_props_get();
    glm_ortho(0.0f, (float) gameProperties->resolutionWidth, (float) gameProperties->resolutionHeight, 0.0f, -1.0f, 1.0f, proj);
    shader_use(spriteShader);
    shader_set_mat4_float(spriteShader, "projection", &proj);
    shader_set_int(spriteShader, "sprite", 0);
}

void sprite_renderer_finalize() {}

void sprite_renderer_draw_sprite(const Texture* texture, const Rect2* sourceRect, const Rect2* destRect, const Color* color, bool flipX, bool flipY, TransformModel2D* globalTransform) {
    glDepthMask(false);
//    const Vector2 absScale = {fabs(destRect->w), fabs(destRect->h) };
//    // 1. Translation
//    mat4 model = {
//        {1.0f, 0.0f, 0.0f, 0.0f},
//        {0.0f, 1.0f, 0.0f, 0.0f},
//        {0.0f, 0.0f, 1.0f, 0.0f},
//        {0.0f, 0.0f, 0.0f, 1.0f}
//    };
//    glm_translate(model, (vec3) {
//        destRect->x, destRect->y, 0.0f
//    });
//    // 2. Rotation
//    glm_translate(model, (vec3) {
//        0.5f * absScale.x, 0.5f * absScale.y, 0.0f
//    });
//    glm_make_rad(&rotation);
//    glm_rotate(model, rotation, (vec3) {
//        0.0f, 0.0f, 1.0f
//    });
//    glm_translate(model, (vec3) {
//        -0.5f * absScale.x, -0.5f * absScale.y, 0.0f
//        });
//    // 3. Scaling
//    glm_scale(model, (vec3) {
//        destRect->w, destRect->h, 1.0f
//    });


    glm_scale(globalTransform->model, (vec3) {
        destRect->w, destRect->h, 1.0f
    });

    glBindVertexArray(spriteQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);

    shader_use(spriteShader);
//    shader_set_mat4_float(spriteShader, "models[0]", &model);
    shader_set_mat4_float(spriteShader, "models[0]", &globalTransform->model);
    const int VERTEX_ITEM_COUNT = 1;
    const int NUMBER_OF_VERTICES = 6;
    const float SPRITE_ID = 0.0f;
    TextureCoordinates textureCoords = renderer_get_texture_coordinates(texture, sourceRect, flipX, flipY);

    const float determinate = glm_mat4_det(globalTransform->model);
    const int vertsStride = 9;
    GLfloat verts[54]; // TODO: fix magic number
    for (int i = 0; i < VERTEX_ITEM_COUNT * NUMBER_OF_VERTICES; i++) {
        bool isSMin;
        bool isTMin;
        if (determinate >= 0.0f) {
            isSMin = i == 0 || i == 2 || i == 3 ? true : false;
            isTMin = i == 1 || i == 2 || i == 5 ? true : false;
        } else {
            isSMin = i == 1 || i == 2 || i == 5 ? true : false;
            isTMin = i == 0 || i == 2 || i == 3 ? true : false;
        }
        const int row = i * vertsStride;
        verts[row + 0] = SPRITE_ID;
        verts[row + 1] = isSMin ? 0.0f : 1.0f;
        verts[row + 2] = isTMin ? 0.0f : 1.0f;
        verts[row + 3] = isSMin ? textureCoords.sMin : textureCoords.sMax;
        verts[row + 4] = isTMin ? textureCoords.tMin : textureCoords.tMax;
        verts[row + 5] = color->r;
        verts[row + 6] = color->g;
        verts[row + 7] = color->b;
        verts[row + 8] = color->a;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_ITEM_COUNT * NUMBER_OF_VERTICES);

    renderer_print_opengl_errors();

    glBindVertexArray(0);
    glDepthMask(true);
}

// --- Font Renderer --- //
void font_renderer_initialize() {
    if (FT_Init_FreeType(&rbe_render_context_get()->freeTypeLibrary)) {
        rbe_logger_error("Unable to initialize FreeType library!");
    }
    mat4 proj = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    RBEGameProperties* gameProperties = rbe_game_props_get();
    glm_ortho(0.0f, (float) gameProperties->resolutionWidth, (float) -gameProperties->resolutionHeight, 0.0f, -1.0f, 1.0f, proj);
    fontShader = shader_compile_new_shader(OPENGL_SHADER_SOURCE_VERTEX_FONT, OPENGL_SHADER_SOURCE_FRAGMENT_FONT);
    shader_use(fontShader);
    shader_set_mat4_float(fontShader, "projection", &proj);
}

void font_renderer_finalize() {
    FT_Done_FreeType(rbe_render_context_get()->freeTypeLibrary);
}

void font_renderer_draw_text(const Font* font, const char* text, float x, float y, float scale, const Color* color) {
    Vector2 currentScale = { scale, scale };
    shader_use(fontShader);
    shader_set_vec4_float(fontShader, "textColor", color->r, color->g, color->b, color->a);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->VAO);

    // Iterate through all characters
    char* c = (char*) &text[0];
    const size_t textLength = strlen(text);
    for (size_t i = 0; i < textLength; i++) {
        Character ch = font->characters[(int) *c];
        const float xPos = x + (ch.bearing.x * currentScale.x);
        const float yPos = -y - (ch.size.y - ch.bearing.y) * currentScale.x; // Invert Y because othographic projection is flipped
        const float w = ch.size.x * currentScale.x;
        const float h = ch.size.y * currentScale.y;
        // Update VBO for each characters
        GLfloat verts[6][4] = {
            {xPos,     yPos + h, 0.0f, 0.0f},
            {xPos,     yPos,     0.0f, 1.0f},
            {xPos + w, yPos,     1.0f, 1.0f},

            {xPos,     yPos + h, 0.0f, 0.0f},
            {xPos + w, yPos,     1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureId);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        // Render
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (float) (ch.advance >> 6) * currentScale.x; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        ++c;
    }
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// --- Misc --- //
TextureCoordinates renderer_get_texture_coordinates(const Texture* texture, const Rect2* drawSource, bool flipX, bool flipY) {
    // S
    GLfloat sMin, sMax;
    if (flipX) {
        sMax = (drawSource->x + 0.5f) / (float) texture->width;
        sMin = (drawSource->x + drawSource->w - 0.5f) / (float) texture->width;
    } else {
        sMin = (drawSource->x + 0.5f) / (float) texture->width;
        sMax = (drawSource->x + drawSource->w - 0.5f) / (float) texture->width;
    }
    // T
    GLfloat tMin, tMax;
    if (flipY) {
        tMax = (drawSource->y + 0.5f) / (float) texture->height;
        tMin = (drawSource->y + drawSource->h - 0.5f) / (float) texture->height;
    } else {
        tMin = (drawSource->y + 0.5f) / (float) texture->height;
        tMax = (drawSource->y + drawSource->h - 0.5f) / (float) texture->height;
    }
    TextureCoordinates textureCoords = { sMin, sMax, tMin, tMax };
    return textureCoords;
}

void renderer_print_opengl_errors() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("err = %d\n", err);
        switch (err) {
        case GL_NO_ERROR:
            printf("GL_NO_ERROR\n");
            break;
        case GL_INVALID_ENUM:
            printf("GL_INVALID_ENUM\n");
            break;
        case GL_INVALID_VALUE:
            printf("GL_INVALID_VALUE\n");
            break;
        case GL_INVALID_OPERATION:
            printf("GL_INVALID_OPERATION\n");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            printf("GL_INVALID_FRAMEBUFFER_OPERATION\n");
            break;
        default:
            printf("default\n");
            break;
        }
    }
}
