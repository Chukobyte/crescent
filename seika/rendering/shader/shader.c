#include "shader.h"

#include <stdio.h>

#include "../../utils/logger.h"
#include "../../memory/se_mem.h"

bool shader_check_compile_errors(unsigned int shaderId, const char* type);

SEShader* se_shader_compile_new_shader(const char* vertexSource, const char* fragmentSource) {
    SEShader* shader = SE_MEM_ALLOCATE(SEShader);
    GLuint vertex, fragment;
    // vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    shader_check_compile_errors(vertex, SE_SHADER_VERTEX_TYPE);
    // fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    if (!shader_check_compile_errors(vertex, SE_SHADER_FRAGMENT_TYPE)) {
        return NULL;
    }
    // attack and link shaders
    shader->id = glCreateProgram();
    glAttachShader(shader->id, vertex);
    glAttachShader(shader->id, fragment);
    glLinkProgram(shader->id);
    if (!shader_check_compile_errors(shader->id, SE_SHADER_PROGRAM_TYPE)) {
        return NULL;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shader;
}

void se_shader_destroy(SEShader* shader) {
    SE_MEM_FREE(shader);
}

bool shader_check_compile_errors(unsigned int shaderId, const char* type) {
    GLint success;
    char infoLog[1024];
    if(type == SE_SHADER_PROGRAM_TYPE) {
        glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shaderId, 1024, NULL, infoLog);
            se_logger_error("Shader type '%s' linking failed!\nInfoLog = %s", type, &infoLog);
        }
    } else {
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
            se_logger_error("Shader type '%s' compilation failed!InfoLog = \n%s", type, &infoLog);
        }
    }
    return success;
}

void se_shader_use(SEShader* shader) {
    glUseProgram(shader->id);
}

void se_shader_set_bool(SEShader* shader, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(shader->id, name), (int)value);
}

void se_shader_set_int(SEShader* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void se_shader_set_float(SEShader* shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->id, name), value);
}

void se_shader_set_vec2_float(SEShader* shader, const char* name, float v1, float v2) {
    glUniform2f(glGetUniformLocation(shader->id, name), v1, v2);
}

void se_shader_set_vec3_float(SEShader* shader, const char* name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(shader->id, name), v1, v2, v3);
}

void se_shader_set_vec4_float(SEShader* shader, const char* name, float v1, float v2, float v3, float v4) {
    glUniform4f(glGetUniformLocation(shader->id, name), v1, v2, v3, v4);
}

void se_shader_set_mat4_float(SEShader* shader, const char* name, mat4* value) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, (float*)value);
}
