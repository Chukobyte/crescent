#pragma once

#include "../seika/src/math/se_math.h"

#include "tr_texture.h"

void tr_renderer_init(int windowWidth, int windowHeight, int resolutionWidth, int resolutionHeight);
void tr_renderer_resize(int windowWidth, int windowHeight);
void tr_renderer_draw_sprite(TRTexture* texture, SEVector2* position, SESize2Di* drawSize);
void tr_renderer_print_opengl_errors();
