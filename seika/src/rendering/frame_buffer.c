#include "frame_buffer.h"

#include "shader.h"
#include "../utils/logger.h"
#include "../utils/se_assert.h"
#include "shader_source.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

GLuint frameBuffer = -1;
GLuint textureColorBuffer = -1;
GLuint rbo = -1;
bool hasBeenInitialized = false;

static Shader* screenShader = NULL;
static GLuint screenVAO = -1;
static GLuint screenVBO = -1;

bool se_frame_buffer_initialize() {
    // VAO & VBO
    GLfloat vertices[] = {
        // pos      // tex coords
        -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

    // Initialize render data
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*) NULL);
    // texture coords attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create Framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // Create color attachment
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    // Create renderbuffer object for depth and stencil attachment
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // Check if framebuffer is complete
    const bool success = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (!success) {
        se_logger_error("Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // compile shaders
    screenShader = shader_compile_new_shader(OPENGL_SHADER_SOURCE_VERTEX_SCREEN, OPENGL_SHADER_SOURCE_FRAGMENT_SCREEN);
    shader_use(screenShader);
    shader_set_int(screenShader, "screenTexture", 0);

    hasBeenInitialized = true;
    return success;
}

void se_frame_buffer_finalize() {
    hasBeenInitialized = false;
}

void se_frame_buffer_bind() {
    SE_ASSERT(hasBeenInitialized);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void se_frame_buffer_unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shader* se_frame_buffer_get_screen_shader() {
    return screenShader;
}

unsigned int se_frame_buffer_get_color_buffer_texture() {
    return textureColorBuffer;
}

unsigned int se_frame_buffer_get_quad_vao() {
    return screenVAO;
}
