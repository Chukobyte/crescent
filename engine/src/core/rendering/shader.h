#pragma once

#include <stdbool.h>

#include <cglm/cglm.h>
#include <glad/glad.h>

static const char* SHADER_VERTEX_TYPE   = "VERTEX";
static const char* SHADER_FRAGMENT_TYPE = "FRAGMENT";
static const char* SHADER_PROGRAM_TYPE  = "PROGRAM";

typedef struct Shader {
    GLuint id;
} Shader;

Shader* shader_compile_new_shader(const char* vertexSource, const char* fragmentSource);

void shader_check_compile_errors(unsigned int shaderId, const char* type);

void shader_use(Shader* shader);

void shader_set_bool(Shader* shader, const char* name, bool value);

void shader_set_int(Shader* shader, const char* name, int value);

void shader_set_float(Shader* shader, const char* name, float value);

void shader_set_vec2_float(Shader* shader, const char* name, float v1, float v2);

void shader_set_vec3_float(Shader* shader, const char* name, float v1, float v2, float v3);

void shader_set_vec4_float(Shader* shader, const char* name, float v1, float v2, float v3, float v4);

void shader_set_mat4_float(Shader* shader, const char* name, mat4* value);
