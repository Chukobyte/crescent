#include "shader_file_parser.h"

#include <string.h>

#include "shader_instance.h"
#include "../../utils/se_string_util.h"
#include "../../memory/se_mem.h"

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

void shader_file_parse_data_delete_internal_memory(SEShaderFileParseData* parseData) {
    SE_MEM_FREE(parseData->fullVertexSource);
    SE_MEM_FREE(parseData->fullFragmentSource);
    for (size_t i = 0; i < parseData->functionCount; i++) {
        SE_MEM_FREE(parseData->functions[i].name);
        SE_MEM_FREE(parseData->functions[i].fullFunctionSource);
    }
}

// Will parse the next token, returns true if there is more source to parse
bool shader_file_find_next_token(char** shaderSource, char* tokenOut, bool* semiColonFound) {
    tokenOut[0] = '\0';

    if (semiColonFound) {
        *semiColonFound = false;
    }

    int tokenIndex = 0;
    while (*(*shaderSource) != '\0') {
        if (*(*shaderSource) == ' ' || *(*shaderSource) == ';') {
            if (semiColonFound && *(*shaderSource) == ';') {
                *semiColonFound = true;
            }
            (*shaderSource)++;
            tokenOut[tokenIndex] = '\0';
            return true;
        }
        if (*(*shaderSource) != '\n') {
            tokenOut[tokenIndex++] = *(*shaderSource);
        }
        (*shaderSource)++;
    }
    return false;
}

// Continue parsing shader source and returns 'true' if an '=' was found
bool shader_file_find_next_uniform_equals_token(char** shaderSource) {
    while (*(*shaderSource) != '\0') {
        if (*(*shaderSource) == '=') {
            (*shaderSource)++;
            return true;
        }
        (*shaderSource)++;
    }
    return false;
}

bool shader_file_find_next_uniform_default_value(char** shaderSource, char* tokenOut, bool* semiColonFound) {
    tokenOut[0] = '\0';

    if (semiColonFound) {
        *semiColonFound = false;
    }

    int tokenIndex = 0;
    while (*(*shaderSource) != '\0') {
        if (*(*shaderSource) == ';') {
            if (semiColonFound) {
                *semiColonFound = true;
            }
            (*shaderSource)++;
            tokenOut[tokenIndex] = '\0';
            return true;
        }
        if (*(*shaderSource) != '\n' && *(*shaderSource) != ' ') {
            tokenOut[tokenIndex++] = *(*shaderSource);
        }
        (*shaderSource)++;
    }
    return false;
}

SEShaderFileParserFunction shader_file_find_next_function(char** shaderSource, const char* functionReturnType) {
    SEShaderFileParserFunction parsedFunction = { .name = NULL, .fullFunctionSource = NULL };
    char shaderFunctionBuffer[1024];
    strcpy(shaderFunctionBuffer, functionReturnType);
    strcat(shaderFunctionBuffer, " ");
    unsigned int bufferIndex = strlen(shaderFunctionBuffer);

    // Get name first
    char shaderFunctionName[64];
    unsigned int shaderFunctionNameIndex = 0;
    while (*(*shaderSource) != '\0') {
        shaderFunctionBuffer[bufferIndex++] = *(*shaderSource);
        if (*(*shaderSource) != ' ') {
            shaderFunctionName[shaderFunctionNameIndex++] = *(*shaderSource);
        }
        (*shaderSource)++;
        if (*(*shaderSource) == '(') {
            shaderFunctionName[shaderFunctionNameIndex] = '\0';
            break;
        }
    }
    // Now just loop through until we encounter a '}'
    while (*(*shaderSource) != '\0') {
        shaderFunctionBuffer[bufferIndex++] = *(*shaderSource);
        (*shaderSource)++;
        if (*(*shaderSource) == '}') {
            shaderFunctionBuffer[bufferIndex++] = *(*shaderSource);
            (*shaderSource)++;
            shaderFunctionBuffer[bufferIndex] = '\0';
            break;
        }
    }

    parsedFunction.name = se_strdup(shaderFunctionName);
    parsedFunction.fullFunctionSource = se_strdup(shaderFunctionBuffer);
    return parsedFunction;
}

// Should have a valid function at this point...
char* shader_file_parse_function_body(const char* functionSource) {
    char shaderFunctionBuffer[1024];
    strcpy(shaderFunctionBuffer, functionSource);
    unsigned int functionBufferIndex = 0;
    char currentToken = shaderFunctionBuffer[functionBufferIndex];
    // Find beginning body
    while (currentToken != '{') {
        currentToken = shaderFunctionBuffer[++functionBufferIndex];
    }
    char functionBodyBuffer[1024];
    unsigned int functionBodyIndex = 0;
    currentToken = shaderFunctionBuffer[++functionBufferIndex];
    while (currentToken != '}') {
        currentToken = shaderFunctionBuffer[functionBufferIndex++];
        functionBodyBuffer[functionBodyIndex++] = currentToken;
    }

    return se_strdup(functionBodyBuffer);
}

bool shader_file_is_function_return_type_token(const char* token) {
    return strcmp(token, "void") == 0 || strcmp(token, "bool") == 0 || strcmp(token, "int") == 0
           || strcmp(token, "float") == 0 || strcmp(token, "vec2") == 0 || strcmp(token, "vec3") == 0
           || strcmp(token, "vec4") == 0;
}

SEShaderFileParseResult se_shader_file_parser_parse_shader(const char* shaderSource) {
    SEShaderFileParseResult result = {.errorMessage = {0}, .parsedInstance = NULL};
    char* originalSource = se_strdup(shaderSource);
    char* currentSource = originalSource;
    char shaderToken[32];
    bool isSemicolonFound;
    SEShaderFileParseData parseData = { .shaderType = SEShaderInstanceType_INVALID,
                                        .fullVertexSource = NULL,
                                        .fullFragmentSource = NULL, .vertexFunctionSource = NULL,
                                        .fragmentFunctionSource = NULL,
                                        .uniformCount = 0,
                                        .functionCount = 0
                                      };

    // Parse shader type
    shader_file_find_next_token(&currentSource, shaderToken, &isSemicolonFound);
    if (strcmp(shaderToken, "shader_type") != 0) {
        strcpy(result.errorMessage, "Didn't find 'shader_type' first line!");
        SE_MEM_FREE(originalSource);
        return result;
    }
//    printf("shader token = %s\n", shaderToken);
    // Parse shader type value
    shader_file_find_next_token(&currentSource, shaderToken, &isSemicolonFound);
    if (strcmp(shaderToken, "screen") == 0) {
        parseData.shaderType = SEShaderInstanceType_SCREEN;
    } else {
        strcpy(result.errorMessage, "Didn't find 'shader_type' value on first line!");
        SE_MEM_FREE(originalSource);
        return result;
    }
//    printf("shader token = %s\n", shaderToken);

    // Parse the rest
    while (shader_file_find_next_token(&currentSource, shaderToken, &isSemicolonFound)) {
        printf("\nshader token = '%s'\n\n", shaderToken);
        // UNIFORM VARIABLE
        if (strcmp(shaderToken, "uniform") == 0) {
            printf("Parsing shader uniform variable\n");
            SEShaderParam shaderUniform;
            // Parse uniform type
            char shaderUniformTypeName[32];
            shader_file_find_next_token(&currentSource, shaderUniformTypeName, &isSemicolonFound);
            if (strcmp(shaderUniformTypeName, "int") == 0) {
                shaderUniform.type = SEShaderParamType_INT;
                shaderUniform.value.intValue = 0;
            } else if (strcmp(shaderUniformTypeName, "float") == 0) {
                shaderUniform.type = SEShaderParamType_FLOAT;
                shaderUniform.value.floatValue = 0.0f;
            } else if (strcmp(shaderUniformTypeName, "vec2") == 0) {
                shaderUniform.type = SEShaderParamType_FLOAT2;
                shaderUniform.value.float2Value = (SEVector2) {
                    0.0f, 0.0f
                };
            } else {
                strcpy(result.errorMessage, "Expected to find uniform shader type!");
                SE_MEM_FREE(originalSource);
                return result;
            }
            printf("type = '%s'\n", shaderUniformTypeName);
            // Parse uniform name
            char shaderUniformName[32];
            shader_file_find_next_token(&currentSource, shaderUniformName, &isSemicolonFound);
            printf("name = '%s'\n", shaderUniformName);
            // If we didn't find the semicolon, parse for default value
            if (!isSemicolonFound) {
                printf("Parsing shader default value\n");
                // Look for '='
                if (!shader_file_find_next_uniform_equals_token(&currentSource)) {
                    strcpy(result.errorMessage, "Expected to find an '=' when declaring a uniform variable!");
                    SE_MEM_FREE(originalSource);
                    return result;
                }
                printf("Found '='!\n");
                // Now parse default value
                char shaderUniformDefaultValue[32];
                shader_file_find_next_uniform_default_value(&currentSource, shaderUniformDefaultValue, &isSemicolonFound);
                if (!isSemicolonFound) {
                    strcpy(result.errorMessage, "Expected to find a ';' after declaring a uniform variable's default value!");
                    SE_MEM_FREE(originalSource);
                    return result;
                }
                printf("shader uniform default value = '%s'\n", shaderUniformDefaultValue);
                switch (shaderUniform.type) {
                case SEShaderParamType_INT: {
                    char* endptr = NULL;
                    shaderUniform.value.intValue = (int) strtol(shaderUniformDefaultValue, &endptr, 10);
                    if (*endptr != '\0') {
                        strcpy(result.errorMessage, "Not a valid uniform int default value!");
                        SE_MEM_FREE(originalSource);
                        return result;
                    }
                    printf("Set int default value to '%d'\n", shaderUniform.value.intValue);
                    break;
                }
                case SEShaderParamType_FLOAT: {
                    shaderUniform.value.floatValue = strtof(shaderUniformDefaultValue, NULL);
                    printf("Set float default value to '%f'\n", shaderUniform.value.floatValue);
                    break;
                }
                default:
                    break;
                }
            }
            // Finally after all validation checks, add new uniform to array
            parseData.uniforms[parseData.uniformCount++] = shaderUniform;
        } else if (shader_file_is_function_return_type_token(shaderToken)) {
            SEShaderFileParserFunction parsedFunction = shader_file_find_next_function(&currentSource, shaderToken);
            if (parsedFunction.name == NULL || parsedFunction.fullFunctionSource == NULL) {
                strcpy(result.errorMessage, "Didn't successfully parse shader function!");
                SE_MEM_FREE(originalSource);
                return result;
            }
            printf("function name = '%s'\n", parsedFunction.name);
            printf("function source = '%s'\n", parsedFunction.fullFunctionSource);
            // Check for vertex and fragment shader functions
            if (strcmp(parsedFunction.name, "vertex") == 0) {
                parseData.vertexFunctionSource = shader_file_parse_function_body(parsedFunction.fullFunctionSource);
                SE_MEM_FREE(parsedFunction.name);
                SE_MEM_FREE(parsedFunction.fullFunctionSource);
            } else if (strcmp(parsedFunction.name, "fragment") == 0) {
                parseData.fragmentFunctionSource = shader_file_parse_function_body(parsedFunction.fullFunctionSource);
                SE_MEM_FREE(parsedFunction.name);
                SE_MEM_FREE(parsedFunction.fullFunctionSource);
            } else {
                // Add non vertex and fragment functions to our array
                parseData.functions[parseData.functionCount++] = parsedFunction;
            }
        } else {
            strcpy(result.errorMessage, "Unexpected token!");
            SE_MEM_FREE(originalSource);
            return result;
        }
    }

    // Now that we've parsed everything create vertex and fragment source text


    SE_MEM_FREE(originalSource);

    return result;
}
