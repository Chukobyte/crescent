#include "shader_file_parser.h"

#include <string.h>

#include "shader_source.h"
#include "../../utils/se_string_util.h"
#include "../../memory/se_mem.h"

#define SHADER_FILE_PARSER_ERROR_RETURN(RESULT, SOURCE, MESSAGE) \
strcpy((RESULT).errorMessage, (MESSAGE));                        \
SE_MEM_FREE((SOURCE));                                           \
return (RESULT);

#define SHADER_FILE_PARSER_ERROR_FMT_RETURN(RESULT, SOURCE, FMT, ...) \
sprintf((RESULT).errorMessage, (FMT), ##__VA_ARGS__);                 \
SE_MEM_FREE((SOURCE));                                                \
return (RESULT);

char* shader_file_parse_data_get_full_uniforms_source(SEShaderFileParseData* parseData) {
    if (parseData->uniformCount == 0) {
        return NULL;
    }
    char uniformsBuffer[1024];
    uniformsBuffer[0] = '\0';
    for (size_t i = 0; i < parseData->uniformCount; i++) {
        strcat(uniformsBuffer, "uniform ");
        switch (parseData->uniforms[i].type) {
        case SEShaderParamType_BOOL: {
            strcat(uniformsBuffer, "bool ");
            break;
        }
        case SEShaderParamType_INT: {
            strcat(uniformsBuffer, "int ");
            break;
        }
        case SEShaderParamType_FLOAT: {
            strcat(uniformsBuffer, "float ");
            break;
        }
        case SEShaderParamType_FLOAT2: {
            strcat(uniformsBuffer, "vec2 ");
            break;
        }
        default:
            break;
        }
        strcat(uniformsBuffer, parseData->uniforms[i].name);
        strcat(uniformsBuffer, ";\n");
    }
    return se_strdup(uniformsBuffer);
}

char* shader_file_parse_data_get_full_functions_source(SEShaderFileParseData* parseData) {
    if (parseData->functionCount == 0) {
        return NULL;
    }
    char functionsBuffer[4096];
    functionsBuffer[0] = '\0';
    for (size_t i = 0; i < parseData->functionCount; i++) {
        strcat(functionsBuffer, parseData->functions[i].fullFunctionSource);
        strcat(functionsBuffer, "\n");
    }
    return se_strdup(functionsBuffer);
}

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
    shaderFunctionBuffer[0] = '\0';
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
    // Return NULL if an empty function with (with no newlines at least)
    if (currentToken == '}') {
        return NULL;
    }
    while (currentToken != '}') {
        currentToken = shaderFunctionBuffer[functionBufferIndex++];
        functionBodyBuffer[functionBodyIndex++] = currentToken;
    }
    functionBodyBuffer[functionBodyIndex - 1] = '\n';
    functionBodyBuffer[functionBodyIndex] = '\0';

    return se_strdup(functionBodyBuffer);
}

bool shader_file_is_function_return_type_token(const char* token) {
    return strcmp(token, "void") == 0 || strcmp(token, "bool") == 0 || strcmp(token, "int") == 0
           || strcmp(token, "float") == 0 || strcmp(token, "vec2") == 0 || strcmp(token, "vec3") == 0
           || strcmp(token, "vec4") == 0;
}

// TODO: Make a macro for error returns
SEShaderFileParseResult se_shader_file_parser_parse_shader(const char* shaderSource) {
    SEShaderFileParseResult result = { .errorMessage = {0} };
    char* originalSource = se_strdup(shaderSource);
    char* currentSource = originalSource;
    char shaderToken[32];
    bool isSemicolonFound;
    result.parseData = (SEShaderFileParseData) {
        .shaderType = SEShaderInstanceType_INVALID,
        .fullVertexSource = NULL,
        .fullFragmentSource = NULL, .vertexFunctionSource = NULL,
        .fragmentFunctionSource = NULL,
        .uniformCount = 0,
        .functionCount = 0
    };

    // Parse shader type
    shader_file_find_next_token(&currentSource, shaderToken, &isSemicolonFound);
    if (strcmp(shaderToken, "shader_type") != 0) {
        SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Didn't find 'shader_type' first line!  Found '%s'", shaderToken);
    }
    // Parse shader type value
    shader_file_find_next_token(&currentSource, shaderToken, &isSemicolonFound);
    if (strcmp(shaderToken, "screen") == 0) {
        result.parseData.shaderType = SEShaderInstanceType_SCREEN;
    } else {
        SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Didn't find 'shader_type' value on first line, instead found '%s'!", shaderToken);
    }

    // Parse the rest
    while (shader_file_find_next_token(&currentSource, shaderToken, &isSemicolonFound)) {
        // UNIFORM VARIABLE
        if (strcmp(shaderToken, "uniform") == 0) {
//            printf("Parsing shader uniform variable\n");
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
                SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Expected to find uniform shader type, instead found '%s'!", shaderToken);
            }
//            printf("type = '%s'\n", shaderUniformTypeName);
            // Parse uniform name
            char shaderUniformName[32];
            shader_file_find_next_token(&currentSource, shaderUniformName, &isSemicolonFound);
            shaderUniform.name = se_strdup(shaderUniformName);
//            printf("name = '%s'\n", shaderUniformName);
            // If we didn't find the semicolon, parse for default value
            if (!isSemicolonFound) {
//                printf("Parsing shader default value\n");
                // Look for '='
                if (!shader_file_find_next_uniform_equals_token(&currentSource)) {
                    SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Expected to find an '=' when declaring a uniform variable, instead found '%s'!", shaderToken);
                }
//                printf("Found '='!\n");
                // Now parse default value
                char shaderUniformDefaultValue[32];
                shader_file_find_next_uniform_default_value(&currentSource, shaderUniformDefaultValue, &isSemicolonFound);
                if (!isSemicolonFound) {
                    SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Expected to find a ';' after declaring a uniform variable's default value, found '%s'!", shaderToken);
                }
//                printf("shader uniform default value = '%s'\n", shaderUniformDefaultValue);
                switch (shaderUniform.type) {
                case SEShaderParamType_INT: {
                    char* endptr = NULL;
                    shaderUniform.value.intValue = (int) strtol(shaderUniformDefaultValue, &endptr, 10);
                    if (*endptr != '\0') {
                        SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Not a valid uniform int default value, found '%s'!", shaderToken);
                    }
//                    printf("Set int default value to '%d'\n", shaderUniform.value.intValue);
                    break;
                }
                case SEShaderParamType_FLOAT: {
                    shaderUniform.value.floatValue = strtof(shaderUniformDefaultValue, NULL);
//                    printf("Set float default value to '%f'\n", shaderUniform.value.floatValue);
                    break;
                }
                default:
                    break;
                }
            }
            // Finally after all validation checks, add new uniform to array
            result.parseData.uniforms[result.parseData.uniformCount++] = shaderUniform;
        } else if (shader_file_is_function_return_type_token(shaderToken)) {
            SEShaderFileParserFunction parsedFunction = shader_file_find_next_function(&currentSource, shaderToken);
            if (parsedFunction.name == NULL || parsedFunction.fullFunctionSource == NULL) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Didn't successfully parse shader function!");
            }
//            printf("function name = '%s'\n", parsedFunction.name);
//            printf("function source = '%s'\n", parsedFunction.fullFunctionSource);
            // Check for vertex and fragment shader functions
            if (strcmp(parsedFunction.name, "vertex") == 0) {
                result.parseData.vertexFunctionSource = shader_file_parse_function_body(parsedFunction.fullFunctionSource);
                SE_MEM_FREE(parsedFunction.name);
                SE_MEM_FREE(parsedFunction.fullFunctionSource);
            } else if (strcmp(parsedFunction.name, "fragment") == 0) {
                result.parseData.fragmentFunctionSource = shader_file_parse_function_body(parsedFunction.fullFunctionSource);
                SE_MEM_FREE(parsedFunction.name);
                SE_MEM_FREE(parsedFunction.fullFunctionSource);
            } else {
                // Add non vertex and fragment functions to our array
                result.parseData.functions[result.parseData.functionCount++] = parsedFunction;
            }
        } else {
            SHADER_FILE_PARSER_ERROR_FMT_RETURN(result, originalSource, "Unexpected token '%s'!", shaderToken);
        }
    }

    // Now that we've parsed everything create vertex and fragment source text
    const char* SHADER_UNIFORMS_REPLACE_TOKEN = "//@@UNIFORMS\n";
    const char* SHADER_FUNCTIONS_REPLACE_TOKEN = "//@@FUNCTIONS\n";
    const char* SHADER_VERTEX_BODY_REPLACE_TOKEN = "//@@vertex()\n";
    const char* SHADER_FRAGMENT_BODY_REPLACE_TOKEN = "//@@fragment()\n";
    const unsigned int SHADER_UNIFORMS_REPLACE_TOKEN_LENGTH = strlen(SHADER_UNIFORMS_REPLACE_TOKEN);
    const unsigned int SHADER_FUNCTIONS_REPLACE_TOKEN_LENGTH = strlen(SHADER_FUNCTIONS_REPLACE_TOKEN);
    const unsigned int SHADER_VERTEX_BODY_REPLACE_TOKEN_LENGTH = strlen(SHADER_VERTEX_BODY_REPLACE_TOKEN);
    const unsigned int SHADER_FRAGMENT_BODY_REPLACE_TOKEN_LENGTH = strlen(SHADER_FRAGMENT_BODY_REPLACE_TOKEN);
    char fullShaderBuffer[4096];
    switch (result.parseData.shaderType) {
    case SEShaderInstanceType_SCREEN: {
        strcpy(fullShaderBuffer, SE_OPENGL_SHADER_SOURCE_VERTEX_SCREEN);
        if (result.parseData.vertexFunctionSource) {
            // Vertex uniforms
            char* foundUniformsToken = strstr(fullShaderBuffer, SHADER_UNIFORMS_REPLACE_TOKEN);
            if (!foundUniformsToken) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Unable to find uniforms token in vertex shader!");
            }
            char* uniformsSource = shader_file_parse_data_get_full_uniforms_source(&result.parseData);
            if (uniformsSource) {
                const unsigned int uniformsReplaceLength = strlen(uniformsSource);
                memmove(foundUniformsToken + uniformsReplaceLength,
                        foundUniformsToken + SHADER_UNIFORMS_REPLACE_TOKEN_LENGTH,
                        strlen(foundUniformsToken + SHADER_UNIFORMS_REPLACE_TOKEN_LENGTH) + 1);
                memcpy(foundUniformsToken, uniformsSource, uniformsReplaceLength);
                SE_MEM_FREE(uniformsSource);
            }
            // Vertex functions
            char* foundFunctionsToken = strstr(fullShaderBuffer, SHADER_FUNCTIONS_REPLACE_TOKEN);
            if (!foundFunctionsToken) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Unable to find functions token in vertex shader!");
            }
            char* functionsSource = shader_file_parse_data_get_full_functions_source(&result.parseData);
            if (functionsSource) {
                const unsigned int functionsReplaceLength = strlen(functionsSource);
                memmove(foundFunctionsToken + functionsReplaceLength,
                        foundFunctionsToken + SHADER_FUNCTIONS_REPLACE_TOKEN_LENGTH,
                        strlen(foundFunctionsToken + SHADER_FUNCTIONS_REPLACE_TOKEN_LENGTH) + 1);
                memcpy(foundFunctionsToken, functionsSource, functionsReplaceLength);
                SE_MEM_FREE(functionsSource);
            }
            // Vertex body
            char* foundVertexToken = strstr(fullShaderBuffer, SHADER_VERTEX_BODY_REPLACE_TOKEN);
            if (!foundVertexToken) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Unable to find vertex() token in vertex shader!")
            }
            const unsigned int vertexBodyReplaceLength = strlen(result.parseData.vertexFunctionSource);
            memmove(foundVertexToken + vertexBodyReplaceLength,
                    foundVertexToken + SHADER_VERTEX_BODY_REPLACE_TOKEN_LENGTH,
                    strlen(foundVertexToken + SHADER_VERTEX_BODY_REPLACE_TOKEN_LENGTH) + 1);
            memcpy(foundVertexToken, result.parseData.vertexFunctionSource, vertexBodyReplaceLength);
        }
//        printf("FULL VERTEX SOURCE = \n%s\n", fullShaderBuffer);
        result.parseData.fullVertexSource = se_strdup(fullShaderBuffer);

        strcpy(fullShaderBuffer, SE_OPENGL_SHADER_SOURCE_FRAGMENT_SCREEN);
        if (result.parseData.fragmentFunctionSource) {
            // Fragment uniforms
            char* foundUniformsToken = strstr(fullShaderBuffer, SHADER_UNIFORMS_REPLACE_TOKEN);
            if (!foundUniformsToken) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Unable to find uniforms token in fragment shader!")
            }
            char* uniformsSource = shader_file_parse_data_get_full_uniforms_source(&result.parseData);
            if (uniformsSource) {
                const unsigned int uniformsReplaceLength = strlen(uniformsSource);
                memmove(foundUniformsToken + uniformsReplaceLength,
                        foundUniformsToken + SHADER_UNIFORMS_REPLACE_TOKEN_LENGTH,
                        strlen(foundUniformsToken + SHADER_UNIFORMS_REPLACE_TOKEN_LENGTH) + 1);
                memcpy(foundUniformsToken, uniformsSource, uniformsReplaceLength);
                SE_MEM_FREE(uniformsSource);
            }
            // Fragment functions
            char* foundFunctionsToken = strstr(fullShaderBuffer, SHADER_FUNCTIONS_REPLACE_TOKEN);
            if (!foundFunctionsToken) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Unable to find functions token in fragment shader!")
            }
            char* functionsSource = shader_file_parse_data_get_full_functions_source(&result.parseData);
            if (functionsSource) {
                const unsigned int functionsReplaceLength = strlen(functionsSource);
                memmove(foundFunctionsToken + functionsReplaceLength,
                        foundFunctionsToken + SHADER_FUNCTIONS_REPLACE_TOKEN_LENGTH,
                        strlen(foundFunctionsToken + SHADER_FUNCTIONS_REPLACE_TOKEN_LENGTH) + 1);
                memcpy(foundFunctionsToken, functionsSource, functionsReplaceLength);
                SE_MEM_FREE(functionsSource);
            }
            // Fragment body
            char* foundFragmentToken = strstr(fullShaderBuffer, SHADER_FRAGMENT_BODY_REPLACE_TOKEN);
            if (!foundFragmentToken) {
                SHADER_FILE_PARSER_ERROR_RETURN(result, originalSource, "Unable to find fragment() token in fragment shader!")
            }
            const unsigned int fragmentBodyReplaceLength = strlen(result.parseData.fragmentFunctionSource);
            memmove(foundFragmentToken + fragmentBodyReplaceLength,
                    foundFragmentToken + SHADER_FRAGMENT_BODY_REPLACE_TOKEN_LENGTH,
                    strlen(foundFragmentToken + SHADER_FRAGMENT_BODY_REPLACE_TOKEN_LENGTH) + 1);
            memcpy(foundFragmentToken, result.parseData.fragmentFunctionSource, fragmentBodyReplaceLength);
        }
//        printf("FULL FRAGMENT SOURCE = \n%s\n", fullShaderBuffer);
        result.parseData.fullFragmentSource = se_strdup(fullShaderBuffer);
        break;
    }
    default:
        break;
    }

    SE_MEM_FREE(originalSource);

    return result;
}

void se_shader_file_parse_clear_parse_result(SEShaderFileParseResult* result) {}
