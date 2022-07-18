#pragma once

#include <stdbool.h>

#include "texture.h"
#include "font.h"
#include "../math/rbe_math.h"
#include "../ecs/component/transform2d_component.h"

void rbe_renderer_initialize();
void rbe_renderer_finalize();
void rbe_renderer_queue_sprite_draw_call(Texture* texture, Rect2 sourceRect, Size2D destSize, Color color, bool flipX, bool flipY, TransformModel2D* globalTransform);
void rbe_renderer_queue_font_draw_call(Font* font, const char* text, float x, float y, float scale, Color color);
void rbe_renderer_flush_batches();
