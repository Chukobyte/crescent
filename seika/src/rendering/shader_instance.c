#include "shader_instance.h"

#include <string.h>

#include "../memory/se_mem.h"
#include "../utils/logger.h"

ShaderInstance* se_shader_instance_create(const char* shaderDefinitionPath) {
    ShaderInstance* instance = SE_MEM_ALLOCATE(ShaderInstance);
    // TODO: Look for and compile vertex and fragment shaders from shader definition
    instance->paramMap = se_string_hash_map_create_default_capacity();
    // TODO: Load shader params
    return instance;
}

void se_shader_instance_destroy(ShaderInstance* shaderInstance) {
    SE_STRING_HASH_MAP_FOR_EACH(shaderInstance->paramMap, iter) {
        StringHashMapNode* node = iter.pair;
        ShaderParam* param = (ShaderParam*) *(ShaderParam**) node->value;
        SE_MEM_FREE(param->value);
    }
    se_string_hash_map_destroy(shaderInstance->paramMap);
    SE_MEM_FREE(shaderInstance);
}

void se_shader_instance_set_param_value(ShaderInstance* shaderInstance, const char* name, void* value) {
    if (!se_string_hash_map_has(shaderInstance->paramMap, name)) {
        se_logger_warn("Trying to set shader param for '%s' but it doesn't exist!", name);
        return;
    }
    ShaderParam* param = (ShaderParam*) *(ShaderParam**) se_string_hash_map_get(shaderInstance->paramMap, name);
    memcpy(param->value, value, sizeof(param->value));
}

void* se_shader_instance_get_param_value(ShaderInstance* shaderInstance, const char* name) {
    if (!se_string_hash_map_has(shaderInstance->paramMap, name)) {
        se_logger_warn("Trying to set shader param for '%s' but it doesn't exist!", name);
        return NULL;
    }
    ShaderParam* param = (ShaderParam*) *(ShaderParam**) se_string_hash_map_get(shaderInstance->paramMap, name);
    return param->value;
}
