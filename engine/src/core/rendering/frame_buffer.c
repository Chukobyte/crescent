#include "frame_buffer.h"

#include "shader.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"
#include "shader_source.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

GLuint frameBuffer = -1;
GLuint textureColorBuffer = -1;
GLuint rbo = -1;
bool hasBeenInitialized = false;

static Shader* screenShader = NULL;

bool cre_frame_buffer_initialize() {
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
        rbe_logger_error("Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    screenShader = shader_compile_new_shader(OPENGL_SHADER_SOURCE_VERTEX_SPRITE, OPENGL_SHADER_SOURCE_FRAGMENT_SPRITE);
    shader_use(screenShader);
    shader_set_int(screenShader, "screenTexture", 0);

    hasBeenInitialized = true;
    return success;
}

void cre_frame_buffer_finalize() {
    hasBeenInitialized = false;
}

void frame_buffer_clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cre_frame_buffer_bind() {
    RBE_ASSERT(hasBeenInitialized);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    frame_buffer_clear();
}

void cre_frame_buffer_unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shader* cre_frame_buffer_get_screen_shader() {
    return screenShader;
}
