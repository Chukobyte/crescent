#include "renderer.h"

#include <cglm/cglm.h>

#include "render_context.h"
#include "shader.h"
#include "shader_source.h"
#include "../game_properties.h"
#include "../utils/rbe_assert.h"

typedef struct TextureCoordinates {
    GLfloat sMin;
    GLfloat sMax;
    GLfloat tMin;
    GLfloat tMax;
} TextureCoordinates;

void sprite_renderer_initialize();
void sprite_renderer_finalize();
void font_renderer_initialize();
void font_renderer_finalize();
TextureCoordinates renderer_get_texture_coordinates(Texture* texture, Rect2 drawSource, bool flipX, bool flipY);
void renderer_print_opengl_errors();

static GLuint spriteQuadVAO;
static GLuint spriteQuadVBO;

static Shader* spriteShader = NULL;

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

// temp
void draw_sprite(Texture* texture, Rect2 sourceRect, Rect2 destRect, float rotation, Color color, bool flipX, bool flipY) {
    glDepthMask(false);
    // 1. Translation
    mat4 model = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    glm_translate(model, (vec3) {
        destRect.x, destRect.y, 0.0f
    });
    // 2. Rotation
    glm_translate(model, (vec3) {
        0.5f * destRect.w, 0.5f * destRect.h, 0.0f
    });
    glm_make_rad(&rotation);
    glm_rotate(model, rotation, (vec3) {
        0.0f, 0.0f, 1.0f
    });
    glm_translate(model, (vec3) {
        -0.5f * destRect.w, -0.5f * destRect.h, 0.0f
        });
    // 3. Scaling
    glm_scale(model, (vec3) {
        destRect.w, destRect.h, 1.0f
    });

    glBindVertexArray(spriteQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);

    shader_use(spriteShader);
    shader_set_mat4_float(spriteShader, "models[0]", &model);
    const int VERTEX_ITEM_COUNT = 1;
    const int NUMBER_OF_VERTICES = 6;
    const float SPRITE_ID = 0.0f;
    TextureCoordinates textureCoords = renderer_get_texture_coordinates(texture, sourceRect, flipX, flipY);

    GLfloat verts[VERTEX_ITEM_COUNT * NUMBER_OF_VERTICES][9];
    for (int i = 0; i < VERTEX_ITEM_COUNT * NUMBER_OF_VERTICES; i++) {
        bool isSMin = i == 0 || i == 2 || i == 3 ? true : false;
        bool isTMin = i == 1 || i == 2 || i == 5 ? true : false;
        verts[i][0] = SPRITE_ID;
        verts[i][1] = isSMin ? 0.0f : 1.0f;
        verts[i][2] = isTMin ? 0.0f : 1.0f;
        verts[i][3] = isSMin ? textureCoords.sMin : textureCoords.sMax;
        verts[i][4] = isTMin ? textureCoords.tMin : textureCoords.tMax;
        verts[i][5] = color.r;
        verts[i][6] = color.g;
        verts[i][7] = color.b;
        verts[i][8] = color.a;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_ITEM_COUNT * NUMBER_OF_VERTICES);

    renderer_print_opengl_errors();

    glBindVertexArray(0);
    glDepthMask(true);
}

void rbe_renderer_queue_sprite_draw_call(Texture* texture, Rect2 sourceRect, Rect2 destRect, float rotation, Color color, bool flipX, bool flipY) {
    draw_sprite(texture, sourceRect, destRect, rotation, color, flipX, flipY);
}

void rbe_renderer_flush_batches() {}

// --- Sprite Renderer --- //
void sprite_renderer_initialize() {
    GLfloat vertices[] = {
        //id  // positions // texture coordinates // color
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
    glm_ortho(0.0f, (float)gameProperties->resolutionWidth, (float)gameProperties->resolutionHeight, 0.0f, -1.0f, 1.0f, proj);
    shader_use(spriteShader);
    shader_set_mat4_float(spriteShader, "projection", &proj);
    shader_set_int(spriteShader, "sprite", 0);
}

void sprite_renderer_finalize() {}

// --- Font Renderer --- //
void font_renderer_initialize() {}

void font_renderer_finalize() {}

// --- Misc --- //
TextureCoordinates renderer_get_texture_coordinates(Texture* texture, Rect2 drawSource, bool flipX, bool flipY) {
    // S
    GLfloat sMin, sMax;
    if (flipX) {
        sMax = (drawSource.x + 0.5f) / (float) texture->width;
        sMin = (drawSource.x + drawSource.w - 0.5f) / (float) texture->width;
    } else {
        sMin = (drawSource.x + 0.5f) / (float) texture->width;
        sMax = (drawSource.x + drawSource.w - 0.5f) / (float) texture->width;
    }
    // T
    GLfloat tMin, tMax;
    if (flipY) {
        tMax = (drawSource.y + 0.5f) / (float) texture->height;
        tMin = (drawSource.y + drawSource.h - 0.5f) / (float) texture->height;
    } else {
        tMin = (drawSource.y + 0.5f) / (float) texture->height;
        tMax = (drawSource.y + drawSource.h - 0.5f) / (float) texture->height;
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
