#include "renderer.h"

#include <stdlib.h>

#include "render_context.h"
#include "shader/shader.h"
#include "shader/shader_cache.h"
#include "shader/shader_source.h"
#include "../data_structures/se_hash_map.h"
#include "../data_structures/se_static_array.h"
#include "../utils/se_assert.h"

#define SE_RENDER_TO_FRAMEBUFFER
#define SE_RENDER_LAYER_BATCH_MAX 200
#define SE_RENDER_LAYER_BATCH_ITEM_MAX (SE_RENDER_LAYER_BATCH_MAX / 2)
#define SE_RENDER_TEXTURE_LAYER_TEXTURE_MAX 64

#ifdef SE_RENDER_TO_FRAMEBUFFER
#include "frame_buffer.h"
#endif

typedef struct TextureCoordinates {
    GLfloat sMin;
    GLfloat sMax;
    GLfloat tMin;
    GLfloat tMax;
} TextureCoordinates;

TextureCoordinates renderer_get_texture_coordinates(const SETexture* texture, const SERect2* drawSource, bool flipH, bool flipV);
void renderer_set_shader_instance_params(SEShaderInstance* shaderInstance);
void renderer_print_opengl_errors();

void sprite_renderer_initialize();
void sprite_renderer_finalize();
void sprite_renderer_update_resolution();

void font_renderer_initialize();
void font_renderer_finalize();
void font_renderer_update_resolution();
void font_renderer_draw_text(const SEFont* font, const char* text, float x, float y, float scale, const SEColor* color);

static GLuint spriteQuadVAO;
static GLuint spriteQuadVBO;

static SEShader* spriteShader = NULL;
static SEShader* fontShader = NULL;

static float resolutionWidth = 800.0f;
static float resolutionHeight = 600.0f;
static mat4 spriteProjection = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}
};

// Sprite Batching
typedef struct SpriteBatchItem {
    SETexture* texture;
    SERect2 sourceRect;
    SESize2D destSize;
    SEColor color;
    bool flipH;
    bool flipV;
    SETransformModel2D* globalTransform;
    SEShaderInstance* shaderInstance;
} SpriteBatchItem;

typedef struct FontBatchItem {
    SEFont* font;
    const char* text;
    float x;
    float y;
    float scale;
    SEColor color;
} FontBatchItem;

void renderer_batching_draw_sprites(SpriteBatchItem items[], size_t spriteCount);

// Render Layer - Arranges draw order by z index
typedef struct RenderTextureLayer {
    SpriteBatchItem spriteBatchItems[SE_RENDER_LAYER_BATCH_ITEM_MAX];
    size_t spriteBatchItemCount;
} RenderTextureLayer;

typedef struct RenderLayer {
    RenderTextureLayer renderTextureLayers[SE_RENDER_TEXTURE_LAYER_TEXTURE_MAX];
    FontBatchItem fontBatchItems[SE_RENDER_LAYER_BATCH_ITEM_MAX];
    size_t renderTextureLayerCount;
    size_t fontBatchItemCount;
} RenderLayer;

SE_STATIC_ARRAY_CREATE(RenderLayer, SE_RENDER_LAYER_BATCH_MAX, render_layer_items);
SE_STATIC_ARRAY_CREATE(int, SE_RENDER_LAYER_BATCH_MAX, active_render_layer_items_indices);

// Renderer
void se_renderer_initialize(int inWindowWidth, int inWindowHeight, int inResolutionWidth, int inResolutionHeight) {
    resolutionWidth = (float) inResolutionWidth;
    resolutionHeight = (float) inResolutionHeight;
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    se_render_context_initialize();
    se_renderer_update_window_size((float) inWindowWidth, (float) inWindowHeight);
    sprite_renderer_initialize();
    font_renderer_initialize();
    se_shader_cache_initialize();
#ifdef SE_RENDER_TO_FRAMEBUFFER
    // Initialize framebuffer
    SE_ASSERT_FMT(se_frame_buffer_initialize(inWindowWidth, inWindowHeight), "Framebuffer didn't initialize!");
#endif
    // Set initial data for render layer
    for (size_t i = 0; i < SE_RENDER_LAYER_BATCH_MAX; i++) {
        render_layer_items[i].renderTextureLayerCount = 0;
        render_layer_items[i].fontBatchItemCount = 0;
        for (size_t j = 0; j < SE_RENDER_TEXTURE_LAYER_TEXTURE_MAX; j++) {
            render_layer_items[i].renderTextureLayers[j].spriteBatchItemCount = 0;
        }
    }
}

void se_renderer_finalize() {
    font_renderer_finalize();
    sprite_renderer_finalize();
    se_render_context_finalize();
    se_shader_cache_finalize();
#ifdef SE_RENDER_TO_FRAMEBUFFER
    se_frame_buffer_finalize();
#endif
}

void se_renderer_update_window_size(float windowWidth, float windowHeight) {
    const int width = (int) windowWidth;
    const int height = (int) windowHeight;
    SERenderContext* renderContext = se_render_context_get();
    renderContext->windowWidth = width;
    renderContext->windowHeight = height;
#ifdef SE_RENDER_TO_FRAMEBUFFER
    se_frame_buffer_resize_texture(width, height);
#endif
}

void update_active_render_layer_index(int zIndex) {
    const size_t sizeBefore = SE_STATIC_ARRAY_SIZE(active_render_layer_items_indices);
    SE_STATIC_ARRAY_ADD_IF_UNIQUE(active_render_layer_items_indices, zIndex);
    const size_t sizeAfter = SE_STATIC_ARRAY_SIZE(active_render_layer_items_indices);
    if (sizeBefore != sizeAfter) {
        SE_STATIC_ARRAY_SORT_INT(active_render_layer_items_indices);
    }
}

void se_renderer_queue_sprite_draw_call(SETexture* texture, SERect2 sourceRect, SESize2D destSize, SEColor color, bool flipH, bool flipV, SETransformModel2D* globalTransform, int zIndex, SEShaderInstance* shaderInstance) {
    if (texture == NULL) {
        se_logger_error("NULL texture, not submitting draw call!");
        return;
    }
    SpriteBatchItem item = { .texture = texture, .sourceRect = sourceRect, .destSize = destSize, .color = color, .flipH = flipH, .flipV = flipV, .globalTransform = globalTransform, .shaderInstance = shaderInstance };
    const int arrayZIndex = se_math_clamp_int(zIndex + SE_RENDER_LAYER_BATCH_MAX / 2, 0, SE_RENDER_LAYER_BATCH_MAX - 1);
    // Get texture layer index for render texture
    size_t textureLayerIndex = render_layer_items[arrayZIndex].renderTextureLayerCount;
    for (size_t i = 0; i < render_layer_items[arrayZIndex].renderTextureLayerCount; i++) {
        if (texture == render_layer_items[arrayZIndex].renderTextureLayers[i].spriteBatchItems[0].texture) {
            textureLayerIndex = i;
            break;
        }
    }
    RenderTextureLayer* textureLayer =  &render_layer_items[arrayZIndex].renderTextureLayers[textureLayerIndex];
    // Increment render texture layer count if first sprite
    if (textureLayer->spriteBatchItemCount == 0) {
        render_layer_items[arrayZIndex].renderTextureLayerCount++;
    }
    textureLayer->spriteBatchItems[textureLayer->spriteBatchItemCount++] = item;
    // Update active render layer indices
    update_active_render_layer_index(arrayZIndex);
}

void se_renderer_queue_font_draw_call(SEFont* font, const char* text, float x, float y, float scale, SEColor color, int zIndex) {
    if (font == NULL) {
        se_logger_error("NULL font, not submitting draw call!");
        return;
    }

    FontBatchItem item = { .font = font, .text = text, .x = x, .y = y, .scale = scale, .color = color };
    const int arrayZIndex = se_math_clamp_int(zIndex + SE_RENDER_LAYER_BATCH_MAX / 2, 0, SE_RENDER_LAYER_BATCH_MAX - 1);
    // Update font batch item on render layer
    render_layer_items[arrayZIndex].fontBatchItems[render_layer_items[arrayZIndex].fontBatchItemCount++] = item;
    // Update active render layer indices
    update_active_render_layer_index(arrayZIndex);
}

void se_renderer_flush_batches() {
    for (size_t i = 0; i < active_render_layer_items_indices_count; i++) {
        const size_t layerIndex = active_render_layer_items_indices[i];
        // Sprite
        for (size_t renderTextureIndex = 0; renderTextureIndex < render_layer_items[layerIndex].renderTextureLayerCount; renderTextureIndex++) {
            RenderTextureLayer* renderTextureLayer = &render_layer_items[layerIndex].renderTextureLayers[renderTextureIndex];
            renderer_batching_draw_sprites(renderTextureLayer->spriteBatchItems, renderTextureLayer->spriteBatchItemCount);
            renderTextureLayer->spriteBatchItemCount = 0;
        }
        render_layer_items[layerIndex].renderTextureLayerCount = 0;
        // Font
        for (size_t fontIndex = 0; fontIndex < render_layer_items[layerIndex].fontBatchItemCount; fontIndex++) {
            font_renderer_draw_text(
                render_layer_items[layerIndex].fontBatchItems[fontIndex].font,
                render_layer_items[layerIndex].fontBatchItems[fontIndex].text,
                render_layer_items[layerIndex].fontBatchItems[fontIndex].x,
                render_layer_items[layerIndex].fontBatchItems[fontIndex].y,
                render_layer_items[layerIndex].fontBatchItems[fontIndex].scale,
                &render_layer_items[layerIndex].fontBatchItems[fontIndex].color
            );
        }
        render_layer_items[layerIndex].fontBatchItemCount = 0;
    }

    SE_STATIC_ARRAY_EMPTY(render_layer_items);
    SE_STATIC_ARRAY_EMPTY(active_render_layer_items_indices);
}

void se_renderer_process_and_flush_batches(const SEColor* backgroundColor) {
#ifdef SE_RENDER_TO_FRAMEBUFFER
    se_frame_buffer_bind();
#endif

    // Clear framebuffer with background color
    glClearColor(backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    se_renderer_flush_batches();

#ifdef SE_RENDER_TO_FRAMEBUFFER
    se_frame_buffer_unbind();

    // Clear screen texture background
    static const SEColor screenBackgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    glClearColor(screenBackgroundColor.r, screenBackgroundColor.g, screenBackgroundColor.b, screenBackgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw screen texture from framebuffer
    SEShaderInstance* screenShaderInstance = se_frame_buffer_get_screen_shader();
    se_shader_use(screenShaderInstance->shader);

    // Apply shader instance params
    renderer_set_shader_instance_params(screenShaderInstance);

    glBindVertexArray(se_frame_buffer_get_quad_vao());
    glBindTexture(GL_TEXTURE_2D, se_frame_buffer_get_color_buffer_texture());	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
}

#ifdef SE_RENDER_TO_FRAMEBUFFER
void se_renderer_process_and_flush_batches_just_framebuffer(const SEColor* backgroundColor) {
    se_frame_buffer_bind();

    // Clear framebuffer with background color
    glClearColor(backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    se_renderer_flush_batches();

    se_frame_buffer_unbind();
}
#endif

// --- Sprite Renderer --- //
#define VERTS_STRIDE 10
void sprite_renderer_initialize() {
    GLfloat vertices[] = {
        //id (1) // positions (2) // texture coordinates (2) // color (4) // is pixel art (1)
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
    };

    // Initialize render data
    glGenVertexArrays(1, &spriteQuadVAO);
    glGenBuffers(1, &spriteQuadVBO);

    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindVertexArray(spriteQuadVAO);
    // id attribute
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, VERTS_STRIDE * sizeof(GLfloat), (void*) NULL);
    glEnableVertexAttribArray(0);
    // position attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTS_STRIDE * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTS_STRIDE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // color attribute
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, VERTS_STRIDE * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
    // is pixel art attribute
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, VERTS_STRIDE * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // compile shaders
    spriteShader = se_shader_compile_new_shader(SE_OPENGL_SHADER_SOURCE_VERTEX_SPRITE,
                   SE_OPENGL_SHADER_SOURCE_FRAGMENT_SPRITE);
    sprite_renderer_update_resolution();
    se_renderer_set_sprite_shader_default_params(spriteShader);
}

void sprite_renderer_finalize() {}

void se_renderer_set_sprite_shader_default_params(SEShader* shader) {
    se_shader_use(shader);
    se_shader_set_int(shader, "TEXTURE", 0);
    se_shader_set_mat4_float(shader, "CRE_PROJECTION", &spriteProjection);
}

void sprite_renderer_update_resolution() {
    glm_mat4_identity(spriteProjection);
    glm_ortho(0.0f, resolutionWidth, resolutionHeight, 0.0f, -1.0f, 1.0f, spriteProjection);
}

void renderer_batching_draw_sprites(SpriteBatchItem items[], size_t spriteCount) {
#define MAX_SPRITE_COUNT 100
#define NUMBER_OF_VERTICES 6
#define VERTEX_BUFFER_SIZE (VERTS_STRIDE * NUMBER_OF_VERTICES * MAX_SPRITE_COUNT)

    if (spriteCount <= 0) {
        return;
    }

    glDepthMask(false);

    glBindVertexArray(spriteQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);

    SETexture* texture = items[0].texture;

    GLfloat verts[VERTEX_BUFFER_SIZE];
    for (size_t i = 0; i < spriteCount; i++) {
        if (items[i].shaderInstance != NULL) {
            se_shader_use(items[i].shaderInstance->shader);
            renderer_set_shader_instance_params(items[i].shaderInstance);
        } else {
            se_shader_use(spriteShader);
        }

        glm_scale(items[i].globalTransform->model, (vec3) {
            items[i].destSize.w, items[i].destSize.h, 1.0f
        });
        const float spriteId = (float) i;
        const float determinate = glm_mat4_det(items[i].globalTransform->model);
        const TextureCoordinates textureCoords = renderer_get_texture_coordinates(texture, &items[i].sourceRect, items[i].flipH, items[i].flipV);
        // concat CRE_MODELS[] string for uniform param
        char modelsBuffer[24];
        sprintf(modelsBuffer, "CRE_MODELS[%zu]", i);
        se_shader_set_mat4_float(spriteShader, modelsBuffer, &items[i].globalTransform->model);

        // Loop over vertices
        for (int j = 0; j < NUMBER_OF_VERTICES; j++) {
            bool isSMin;
            bool isTMin;
            if (determinate >= 0.0f) {
                isSMin = j == 0 || j == 2 || j == 3 ? true : false;
                isTMin = j == 1 || j == 2 || j == 5 ? true : false;
            } else {
                isSMin = j == 1 || j == 2 || j == 5 ? true : false;
                isTMin = j == 0 || j == 2 || j == 3 ? true : false;
            }
            const int row = (j * VERTS_STRIDE) + ((int) i * (VERTS_STRIDE * NUMBER_OF_VERTICES));
            verts[row + 0] = spriteId;
            verts[row + 1] = isSMin ? 0.0f : 1.0f;
            verts[row + 2] = isTMin ? 0.0f : 1.0f;
            verts[row + 3] = isSMin ? textureCoords.sMin : textureCoords.sMax;
            verts[row + 4] = isTMin ? textureCoords.tMin : textureCoords.tMax;
            verts[row + 5] = items[i].color.r;
            verts[row + 6] = items[i].color.g;
            verts[row + 7] = items[i].color.b;
            verts[row + 8] = items[i].color.a;
            verts[row + 9] = (float)texture->applyNearestNeighbor;
        }
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (spriteCount * NUMBER_OF_VERTICES));

    renderer_print_opengl_errors();

    glBindVertexArray(0);
    glDepthMask(true);

#undef MAX_SPRITE_COUNT
#undef NUMBER_OF_VERTICES
#undef VERTEX_BUFFER_SIZE
}
#undef VERTS_STRIDE

// --- Font Renderer --- //
void font_renderer_initialize() {
    if (FT_Init_FreeType(&se_render_context_get()->freeTypeLibrary)) {
        se_logger_error("Unable to initialize FreeType library!");
    }
    fontShader = se_shader_compile_new_shader(SE_OPENGL_SHADER_SOURCE_VERTEX_FONT, SE_OPENGL_SHADER_SOURCE_FRAGMENT_FONT);
    font_renderer_update_resolution();
}

void font_renderer_finalize() {
    FT_Done_FreeType(se_render_context_get()->freeTypeLibrary);
}

void font_renderer_update_resolution() {
    mat4 proj = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    glm_ortho(0.0f, resolutionWidth, -resolutionHeight, 0.0f, -1.0f, 1.0f, proj);
    se_shader_use(fontShader);
    se_shader_set_mat4_float(fontShader, "projection", &proj);
}

void font_renderer_draw_text(const SEFont* font, const char* text, float x, float y, float scale, const SEColor* color) {
    SEVector2 currentScale = { scale, scale };
    se_shader_use(fontShader);
    se_shader_set_vec4_float(fontShader, "textColor", color->r, color->g, color->b, color->a);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->VAO);

    // Iterate through all characters
    char* c = (char*) &text[0];
    const size_t textLength = strlen(text);
    for (size_t i = 0; i < textLength; i++) {
        SECharacter ch = font->characters[(int) *c];
        const float xPos = x + (ch.bearing.x * currentScale.x);
        const float yPos = -y - (ch.size.y - ch.bearing.y) * currentScale.x; // Invert Y because orthographic projection is flipped
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
TextureCoordinates renderer_get_texture_coordinates(const SETexture* texture, const SERect2* drawSource, bool flipH, bool flipV) {
    // S
    GLfloat sMin, sMax;
    if (flipH) {
        sMax = (drawSource->x + 0.5f) / (float) texture->width;
        sMin = (drawSource->x + drawSource->w - 0.5f) / (float) texture->width;
    } else {
        sMin = (drawSource->x + 0.5f) / (float) texture->width;
        sMax = (drawSource->x + drawSource->w - 0.5f) / (float) texture->width;
    }
    // T
    GLfloat tMin, tMax;
    if (flipV) {
        tMax = (drawSource->y + 0.5f) / (float) texture->height;
        tMin = (drawSource->y + drawSource->h - 0.5f) / (float) texture->height;
    } else {
        tMin = (drawSource->y + 0.5f) / (float) texture->height;
        tMax = (drawSource->y + drawSource->h - 0.5f) / (float) texture->height;
    }
    TextureCoordinates textureCoords = { sMin, sMax, tMin, tMax };
    return textureCoords;
}

void renderer_set_shader_instance_params(SEShaderInstance* shaderInstance) {
    if (shaderInstance->paramMap->size > 0) {
        SE_STRING_HASH_MAP_FOR_EACH(shaderInstance->paramMap, iter) {
            StringHashMapNode* node = iter.pair;
            SEShaderParam* param = (SEShaderParam*) node->value;
            switch (param->type) {
            case SEShaderParamType_BOOL: {
                se_shader_set_bool(shaderInstance->shader, param->name, param->value.boolValue);
                break;
            }
            case SEShaderParamType_INT: {
                se_shader_set_int(shaderInstance->shader, param->name, param->value.intValue);
                break;
            }
            case SEShaderParamType_FLOAT: {
                se_shader_set_float(shaderInstance->shader, param->name, param->value.floatValue);
                break;
            }
            case SEShaderParamType_FLOAT2: {
                se_shader_set_vec2_float(shaderInstance->shader, param->name, param->value.float2Value.x,
                                         param->value.float2Value.y);
                break;
            }
            case SEShaderParamType_FLOAT3: {
                se_shader_set_vec3_float(shaderInstance->shader, param->name, param->value.float3Value.x,
                                         param->value.float3Value.y, param->value.float3Value.z);
                break;
            }
            case SEShaderParamType_FLOAT4: {
                se_shader_set_vec4_float(shaderInstance->shader, param->name, param->value.float4Value.x,
                                         param->value.float4Value.y, param->value.float4Value.z,
                                         param->value.float4Value.w);
                break;
            }
            }
        }
    }
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
