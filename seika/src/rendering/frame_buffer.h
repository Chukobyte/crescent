#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool se_frame_buffer_initialize();
void se_frame_buffer_finalize();
void se_frame_buffer_bind();
void se_frame_buffer_unbind();
unsigned int se_frame_buffer_get_color_buffer_texture();
unsigned int se_frame_buffer_get_quad_vao();
void se_frame_buffer_resize_texture(int newWidth, int newHeight);

struct SEShaderInstance* se_frame_buffer_get_screen_shader();
void se_frame_buffer_set_screen_shader(struct SEShaderInstance* shaderInstance);
void se_frame_buffer_reset_to_default_screen_shader();

#ifdef __cplusplus
}
#endif
