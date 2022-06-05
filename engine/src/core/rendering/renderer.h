#pragma once

#include <stdbool.h>

#include "texture.h"
#include "../math/rbe_math.h"

void rbe_renderer_initialize();
void rbe_renderer_queue_sprite_draw_call(Texture* texture, Rect2 sourceRect, Rect2 destRect, float rotation, Color color, bool flipX, bool flipY);
void rbe_renderer_queue_font_batch();
void rbe_renderer_flush_batches();
