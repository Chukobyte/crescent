#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "../math/se_math.h"

bool se_frame_buffer_initialize(int inWindowWidth, int inWindowHeight, int inResolutionWidth, int inResolutionHeight);
void se_frame_buffer_finalize();
void se_frame_buffer_bind();
void se_frame_buffer_unbind();
unsigned int se_frame_buffer_get_color_buffer_texture();
unsigned int se_frame_buffer_get_quad_vao();
void se_frame_buffer_resize_texture(int newWidth, int newHeight);
void se_frame_buffer_set_maintain_aspect_ratio(bool shouldMaintainAspectRatio);

struct SEShaderInstance* se_frame_buffer_get_screen_shader();
void se_frame_buffer_set_screen_shader(struct SEShaderInstance* shaderInstance);
void se_frame_buffer_reset_to_default_screen_shader();

typedef struct FrameBufferViewportData {
    SEVector2i position;
    SESize2Di size;
} FrameBufferViewportData;

FrameBufferViewportData se_frame_buffer_generate_viewport_data(int windowWidth, int windowHeight);
FrameBufferViewportData* se_frame_buffer_get_cached_viewport_data();

#ifdef __cplusplus
}
#endif
