#include "tr_renderer.h"

#include <glad/glad.h>

#include "../seika/src/rendering/shader/shader.h"
#include "../seika/src/utils/se_assert.h"

#define TR_VERTS_STRIDE 4

static const char* TR_OPENGL_SHADER_SOURCE_VERTEX_SPRITE =
        "#version 330 core\n"
        "\n"
        "layout (location = 0) in vec2 VERTEX;\n"
        "layout (location = 1) in vec2 TEXTURE_COORDS;\n"
        "\n"
        "out vec2 UV;\n"
        "\n"
        "uniform sampler2D TEXTURE;\n"
        "uniform mat4 MODEL;\n"
        "uniform mat4 PROJECTION;\n"
        "\n"
        "void main() {\n"
        "    UV = TEXTURE_COORDS;\n"
        "    gl_Position = PROJECTION * MODEL * vec4(VERTEX, 0.0f, 1.0f);\n"
        "}\n";

static const char* TR_OPENGL_SHADER_SOURCE_FRAGMENT_SPRITE =
        "#version 330 core\n"
        "\n"
        "in vec2 UV;\n"
        "\n"
        "out vec4 COLOR;\n"
        "\n"
        "uniform sampler2D TEXTURE;\n"
        "\n"
        "void main() {\n"
        "    COLOR = texture(TEXTURE, UV);\n"
        "}\n";


static mat4 spriteProjection = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
};
static GLuint spriteQuadVAO;
static GLuint spriteQuadVBO;
static SEShader* spriteShader = NULL;

void tr_renderer_init(int resolutionWidth, int resolutionHeight) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat vertices[] = {
            // positions (2) texture coordinates (2)
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
    };

    // Initialize render data
    glGenVertexArrays(1, &spriteQuadVAO);
    glGenBuffers(1, &spriteQuadVBO);

    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindVertexArray(spriteQuadVAO);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, TR_VERTS_STRIDE * sizeof(GLfloat), (GLvoid*) NULL);
    glEnableVertexAttribArray(1);
    // texture coords attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, TR_VERTS_STRIDE * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // compile shaders
    spriteShader = se_shader_compile_new_shader(TR_OPENGL_SHADER_SOURCE_VERTEX_SPRITE,
                                                TR_OPENGL_SHADER_SOURCE_FRAGMENT_SPRITE);
    SE_ASSERT(spriteShader);
    se_shader_use(spriteShader);
    se_shader_set_int(spriteShader, "TEXTURE", 0);
    glm_ortho(0.0f, (float)resolutionWidth, (float)resolutionHeight, 0.0f, -1.0f, 1.0f, spriteProjection);
    se_shader_set_mat4_float(spriteShader, "PROJECTION", &spriteProjection);
}

void tr_renderer_draw_sprite(TRTexture* texture, SEVector2* position, SESize2Di* drawSize) {
    mat4 model;
    glm_mat4_identity(model);
    // 1. Translation
    glm_translate(model, (vec3) {
            position->x, position->y, 0.0f
    });
    // 2. Rotation (None)
    // 3. Scaling
    glm_scale(model, (vec3) {
            (float)drawSize->w, (float)drawSize->h, 1.0f
    });

    glBindVertexArray(spriteQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteQuadVBO);

    se_shader_use(spriteShader);
    se_shader_set_mat4_float(spriteShader, "MODEL", &model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
