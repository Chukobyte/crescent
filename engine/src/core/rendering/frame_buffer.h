#pragma once

#include <stdbool.h>

bool cre_frame_buffer_initialize();
void cre_frame_buffer_finalize();
void cre_frame_buffer_bind();
void cre_frame_buffer_unbind();
struct Shader* cre_frame_buffer_get_screen_shader();
