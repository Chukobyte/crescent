#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool se_frame_buffer_initialize();
void se_frame_buffer_finalize();
void se_frame_buffer_bind();
void se_frame_buffer_unbind();
struct Shader* se_frame_buffer_get_screen_shader();
unsigned int se_frame_buffer_get_color_buffer_texture();
unsigned int se_frame_buffer_get_quad_vao();

#ifdef __cplusplus
}
#endif
