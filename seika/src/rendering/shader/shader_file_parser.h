#pragma once

#include "shader_instance.h"

typedef struct SEShaderFileParserFunction {
    char* name;
    char* fullFunctionSource;
} SEShaderFileParserFunction;

typedef struct SEShaderFileParseData {
    SEShaderInstanceType shaderType;
    char* fullVertexSource;
    char* fullFragmentSource;
    char* vertexFunctionSource;
    char* fragmentFunctionSource;
    size_t uniformCount;
    size_t functionCount;
    SEShaderParam uniforms[32];
    SEShaderFileParserFunction functions[32];
} SEShaderFileParseData;

typedef struct SEShaderFileParseResult {
    char errorMessage[128];
    SEShaderFileParseData  parseData;
} SEShaderFileParseResult;

SEShaderFileParseResult se_shader_file_parser_parse_shader(const char* shaderSource);
void se_shader_file_parse_clear_parse_result(SEShaderFileParseResult* result);
