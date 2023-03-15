#include "frame_buffer.h"

#include "shader/shader.h"
#include "shader/shader_instance.h"
#include "../utils/logger.h"
#include "../utils/se_assert.h"
#include "shader/shader_source.h"

GLuint frameBuffer = -1;
GLuint textureColorBuffer = -1;
GLuint rbo = -1;
bool hasBeenInitialized = false;

static SEShaderInstance defaultScreenShader;
static SEShaderInstance* currentScreenShader = NULL;
static GLuint screenVAO = -1;
static GLuint screenVBO = -1;

static int screenTextureWidth = 800;
static int screenTextureHeight = 600;
static int resolutionWidth = 800;
static int resolutionHeight = 600;
static FrameBufferViewportData cachedViewportData = { .position = { .x = 0, .y = 0 }, .size = { .w = 800, .h = 600 } };
static bool maintainAspectRatio = false;

FrameBufferViewportData se_frame_buffer_generate_viewport_data(int windowWidth, int windowHeight) {
    int framebufferWidth = resolutionWidth; // Original framebuffer width
    int framebufferHeight = resolutionHeight; // Original framebuffer height

    // Calculate the aspect ratio of the game's resolution
    const float game_aspect_ratio = (float)framebufferWidth / (float)framebufferHeight;

    // Calculate the aspect ratio of the window
    const float window_aspect_ratio = (float)windowWidth / (float)windowHeight;

    // Adjust the framebuffer width or height to match the window aspect ratio
    if (maintainAspectRatio && game_aspect_ratio != window_aspect_ratio) {
        framebufferHeight = (int)(windowWidth / game_aspect_ratio);
        framebufferWidth = windowWidth;
        if (framebufferHeight > windowHeight) {
            framebufferHeight = windowHeight;
            framebufferWidth = (int)(windowHeight * game_aspect_ratio);
        }
    } else {
        framebufferWidth = windowWidth;
        framebufferHeight = windowHeight;
    }


    // Calculate the viewport dimensions
    const int viewportX = (windowWidth - framebufferWidth) / 2;
    const int viewportY = (windowHeight - framebufferHeight) / 2;
    const int viewportWidth = framebufferWidth;
    const int viewportHeight = framebufferHeight;

    const FrameBufferViewportData data = {
        .position = { .x = viewportX, .y = viewportY },
        .size = { .w = viewportWidth, .h = viewportHeight }
    };
    cachedViewportData = data;
    return data;
}

bool recreate_frame_buffer_object() {
    // Create Framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // Create color attachment
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenTextureWidth, screenTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    // Create renderbuffer object for depth and stencil attachment
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenTextureWidth, screenTextureHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // Check if framebuffer is complete
    const bool success = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (!success) {
        se_logger_error("Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return success;
}

bool se_frame_buffer_initialize(int inWindowWidth, int inWindowHeight, int inResolutionWidth, int inResolutionHeight) {
    screenTextureWidth = inWindowWidth;
    screenTextureHeight = inWindowHeight;
    resolutionWidth = inResolutionWidth;
    resolutionHeight = inResolutionHeight;
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

    bool success = recreate_frame_buffer_object();

    // compile shaders
    SEShader* screenShader = se_shader_compile_new_shader(SE_OPENGL_SHADER_SOURCE_VERTEX_SCREEN,
                             SE_OPENGL_SHADER_SOURCE_FRAGMENT_SCREEN);
    defaultScreenShader = (SEShaderInstance) {
        .shader = screenShader, .paramMap = se_string_hash_map_create_default_capacity()
    };
    se_frame_buffer_set_screen_shader(&defaultScreenShader);

    hasBeenInitialized = true;
    return success;
}

void se_frame_buffer_finalize() {
    se_string_hash_map_destroy(defaultScreenShader.paramMap);
    defaultScreenShader.paramMap = NULL;
    hasBeenInitialized = false;
}

void se_frame_buffer_bind() {
    SE_ASSERT(hasBeenInitialized);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void se_frame_buffer_unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int se_frame_buffer_get_color_buffer_texture() {
    return textureColorBuffer;
}

unsigned int se_frame_buffer_get_quad_vao() {
    return screenVAO;
}

void se_frame_buffer_resize_texture(int newWidth, int newHeight) {
    screenTextureWidth = newWidth;
    screenTextureHeight = newHeight;
    recreate_frame_buffer_object();
}

SEShaderInstance* se_frame_buffer_get_screen_shader() {
    return currentScreenShader;
}

void se_frame_buffer_set_screen_shader(SEShaderInstance* shaderInstance) {
    SE_ASSERT_FMT(shaderInstance != NULL, "Trying to set screen shader to NULL!");
    currentScreenShader = shaderInstance;
    se_shader_use(currentScreenShader->shader);
    se_shader_set_int(currentScreenShader->shader, "TEXTURE", 0);
}

void se_frame_buffer_reset_to_default_screen_shader() {
    currentScreenShader = &defaultScreenShader;
}
