#include "tr_renderer.h"

#include <stdio.h>

#include <glad/glad.h>

#include "../seika/src/math/se_math.h"

static mat4 spriteProjection = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
};

void tr_renderer_init(int resolutionWidth, int resolutionHeight) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
