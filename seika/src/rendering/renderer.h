#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "texture.h"
#include "font.h"
#include "../math/se_math.h"

void se_renderer_initialize(int inResolutionWidth, int inResolutionHeight);
void se_renderer_finalize();
void se_renderer_queue_sprite_draw_call(Texture* texture, Rect2 sourceRect, Size2D destSize, Color color, bool flipX, bool flipY, TransformModel2D* globalTransform);
void se_renderer_queue_font_draw_call(Font* font, const char* text, float x, float y, float scale, Color color);
void se_renderer_process_and_flush_batches(const Color* backgroundColor);
void se_renderer_process_and_flush_batches_just_framebuffer(const Color* backgroundColor);

#ifdef __cplusplus
}
#endif
