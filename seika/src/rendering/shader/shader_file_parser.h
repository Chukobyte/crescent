#pragma once

typedef struct SEShaderFileParseResult {
    char errorMessage[128];
    struct SEShaderInstance* parsedInstance;
} SEShaderFileParseResult;

SEShaderFileParseResult se_shader_file_parser_parse_shader(const char* shaderSource);
