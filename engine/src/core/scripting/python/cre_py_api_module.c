#include "cre_py_api_module.h"

#include <string.h>

#include "../seika/src/asset/asset_manager.h"
#include "../seika/src/input/input.h"
#include "../seika/src/input/mouse.h"
#include "../seika/src/audio/audio.h"
#include "../seika/src/audio/audio_manager.h"
#include "../seika/src/networking/se_network.h"
#include "../seika/src/memory/se_mem.h"
#include "../seika/src/rendering/frame_buffer.h"
#include "../seika/src/rendering/render_context.h"
#include "../seika/src/rendering/renderer.h"
#include "../seika/src/rendering/shader/shader_cache.h"
#include "../seika/src/asset/asset_file_loader.h"
#include "../seika/src/utils/se_assert.h"
#include "../seika/src/utils/se_file_system_utils.h"
#include "../seika/src/utils/se_string_util.h"

#include "py_cache.h"
#include "py_script_context.h"
#include "../../engine_context.h"
#include "../../game_properties.h"
#include "../../node_event.h"
#include "../../world.h"
#include "../../scripting/script_context.h"
#include "../../physics/collision/collision.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../ecs/ecs_manager.h"
#include "../../ecs/system/ec_system.h"
#include "../../ecs/component/animated_sprite_component.h"
#include "../../ecs/component/color_rect_component.h"
#include "../../ecs/component/script_component.h"
#include "../../ecs/component/sprite_component.h"
#include "../../ecs/component/text_label_component.h"
#include "../../ecs/component/node_component.h"
#include "../../scene/scene_manager.h"
#include "../../ecs/component/parallax_component.h"
#include "../../math/curve_float_manager.h"
#include "../../scene/scene_template_cache.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

//--- Helper Functions ---//
// TODO: May want to move into another location...

SEVector2 py_api_mouse_get_global_position(SEMouse* mouse, SEVector2* offset) {
    CRECamera2D* camera = cre_camera_manager_get_current_camera();
    SEMouse* globalMouse = se_mouse_get();
    CREGameProperties* gameProps = cre_game_props_get();
    SERenderContext* renderContext = se_render_context_get();
    const SEVector2 mouse_pixel_coord = {
        se_math_map_to_range(globalMouse->position.x, 0.0f, (float) renderContext->windowWidth, 0.0f, (float) gameProps->resolutionWidth),
        se_math_map_to_range(globalMouse->position.y, 0.0f, (float) renderContext->windowHeight, 0.0f, (float) gameProps->resolutionHeight)
    };
    const SEVector2 mouseWorldPos = {
        (camera->viewport.x + camera->offset.x + mouse_pixel_coord.x + offset->x) * camera->zoom.x,
        (camera->viewport.y + camera->offset.y + mouse_pixel_coord.y + offset->y) * camera->zoom.y
    };
    return mouseWorldPos;
}

void py_api_update_entity_local_position(CreEntity entity, SEVector2* position) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                          CreComponentDataIndex_TRANSFORM_2D);
    const SEVector2 prevPosition = transformComp->localTransform.position;
    transformComp->localTransform.position.x = position->x;
    transformComp->localTransform.position.y = position->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.position.x != prevPosition.x || transformComp->localTransform.position.y != prevPosition.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void py_api_update_entity_local_scale(CreEntity entity, SEVector2 * scale) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                          CreComponentDataIndex_TRANSFORM_2D);
    const SEVector2 prevScale = transformComp->localTransform.scale;
    transformComp->localTransform.scale.x = scale->x;
    transformComp->localTransform.scale.y = scale->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.scale.x != prevScale.x || transformComp->localTransform.scale.y != prevScale.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void py_api_update_entity_local_rotation(CreEntity entity, float rotation) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                          CreComponentDataIndex_TRANSFORM_2D);
    const float prevRotation = transformComp->localTransform.rotation;
    transformComp->localTransform.rotation = rotation;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.rotation != prevRotation) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

//--- Py Utils ---//
PyObject* cre_py_utils_get_entity_instance(CreEntity entity);

//--- CRE PY API ---//

// Engine
PyObject* cre_py_api_engine_exit(PyObject* self, PyObject* args, PyObject* kwargs) {
    int exitCode;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiEngineExitKWList, &exitCode)) {
        CREEngineContext* engineContext = cre_engine_context_get();
        engineContext->isRunning = false;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_engine_set_target_fps(PyObject* self, PyObject* args, PyObject* kwargs) {
    int targetFPS;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiEngineSetTargetFPSKWList, &targetFPS)) {
        CREEngineContext* engineContext = cre_engine_context_get();
        engineContext->targetFPS = targetFPS;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_engine_get_target_fps(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    return Py_BuildValue("(f)", engineContext->targetFPS);
}

PyObject* cre_py_api_engine_get_average_fps(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    return Py_BuildValue("(f)", engineContext->stats.averageFPS);
}

PyObject* cre_py_api_engine_set_fps_display_enabled(PyObject* self, PyObject* args, PyObject* kwargs) {
    bool isEnabled;
    char* fontUID;
    float positionX;
    float positionY;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "bsff", crePyApiGenericEnabledKWList, &isEnabled, &fontUID, &positionX, &positionY)) {
        cre_ecs_manager_enable_fps_display_entity(isEnabled, strlen(fontUID) < 0 ? fontUID : NULL, positionX, positionY);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_engine_get_global_physics_delta_time(PyObject* self, PyObject* args) {
    return Py_BuildValue("f", CRE_GLOBAL_PHYSICS_DELTA_TIME);
}

PyObject* PyInit_cre_py_API(void) {
    return PyModule_Create(&crePyAPIModDef);
}

// Curve Float
PyObject* cre_py_api_curve_float_create_new(PyObject* self, PyObject* args) {
    return Py_BuildValue("i", cre_curve_float_manager_create_new());
}

PyObject* cre_py_api_curve_float_delete(PyObject* self, PyObject* args, PyObject* kwargs) {
    CurveFloatId curveId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiCurveFloatDeleteKWList, &curveId)) {
        cre_curve_float_manager_delete(curveId);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_curve_float_load_from_file(PyObject* self, PyObject* args, PyObject* kwargs) {
    CurveFloatId curveId;
    char* filePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiCurveFloatLoadFromFileKWList, &curveId, &filePath)) {
        cre_curve_float_manager_load_from_file(curveId, filePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_curve_float_add_point(PyObject* self, PyObject* args, PyObject* kwargs) {
    CurveFloatId curveId;
    double x;
    double y;
    double tangentIn;
    double tangentOut;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "idddd", crePyApiCurveFloatAddPointKWList, &curveId, &x, &y, &tangentIn, &tangentOut)) {
        cre_curve_float_manager_add_point(curveId, x, y, tangentIn, tangentOut);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_curve_float_remove_point(PyObject* self, PyObject* args, PyObject* kwargs) {
    CurveFloatId curveId;
    double x;
    double y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "idd", crePyApiCurveFloatRemovePointKWList, &curveId, &x, &y)) {
        if (cre_curve_float_manager_remove_point(curveId, x, y)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_curve_float_eval(PyObject* self, PyObject* args, PyObject* kwargs) {
    CurveFloatId curveId;
    double t;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "id", crePyApiCurveFloatEvalKWList, &curveId, &t)) {
        return Py_BuildValue("d", cre_curve_float_manager_eval(curveId, t));
    }
    return NULL;
}

// Shader Instance
PyObject* cre_py_api_shader_instance_delete(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericShaderIdKWList, &shaderId)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        if (shaderInstance != NULL) {
            se_shader_cache_remove_instance(shaderId);
            SE_MEM_FREE(shaderInstance);
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_create_bool_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    bool initialValue;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isb", crePyApiShaderInstanceCreateParamKWList, &shaderId, &name, &initialValue)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_create_bool(shaderInstance, name, initialValue);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_set_bool_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    bool value;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isb", crePyApiShaderInstanceSetParamKWList, &shaderId, &name, &value)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_update_bool(shaderInstance, name, value);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_get_bool_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiShaderInstanceGetParamKWList, &shaderId, &name)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        const bool paramValue = se_shader_instance_param_get_bool(shaderInstance, name);
        if (paramValue) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_create_int_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    int initialValue;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isi", crePyApiShaderInstanceCreateParamKWList, &shaderId, &name, &initialValue)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_create_int(shaderInstance, name, initialValue);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_set_int_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    int value;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isi", crePyApiShaderInstanceSetParamKWList, &shaderId, &name, &value)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_update_int(shaderInstance, name, value);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_get_int_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiShaderInstanceGetParamKWList, &shaderId, &name)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        const int paramValue = se_shader_instance_param_get_int(shaderInstance, name);
        return Py_BuildValue("i", paramValue);
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_create_float_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float initialValue;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isf", crePyApiShaderInstanceCreateParamKWList, &shaderId, &name, &initialValue)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_create_float(shaderInstance, name, initialValue);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_set_float_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float value;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isf", crePyApiShaderInstanceSetParamKWList, &shaderId, &name, &value)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_update_float(shaderInstance, name, value);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_get_float_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiShaderInstanceGetParamKWList, &shaderId, &name)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        const float paramValue = se_shader_instance_param_get_float(shaderInstance, name);
        return Py_BuildValue("f", paramValue);
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_create_float2_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float initialValueX;
    float initialValueY;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isff", crePyApiShaderInstanceCreateFloat2ParamKWList, &shaderId, &name, &initialValueX, &initialValueY)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_create_float2(shaderInstance, name, (SEVector2) {
            initialValueX, initialValueY
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_set_float2_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float valueX;
    float valueY;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isff", crePyApiShaderInstanceSetFloat2ParamKWList, &shaderId, &name, &valueX, &valueY)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_update_float2(shaderInstance, name, (SEVector2) {
            valueX, valueY
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_get_float2_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiShaderInstanceGetParamKWList, &shaderId, &name)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        const SEVector2 paramValue = se_shader_instance_param_get_float2(shaderInstance, name);
        return Py_BuildValue("(ff)", paramValue.x, paramValue.y);
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_create_float3_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float initialValueX;
    float initialValueY;
    float initialValueZ;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isfff", crePyApiShaderInstanceCreateFloat3ParamKWList, &shaderId, &name, &initialValueX, &initialValueY, &initialValueZ)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_create_float3(shaderInstance, name, (SEVector3) {
            initialValueX, initialValueY, initialValueZ
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_set_float3_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float valueX;
    float valueY;
    float valueZ;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isfff", crePyApiShaderInstanceSetFloat3ParamKWList, &shaderId, &name, &valueX, &valueY, &valueZ)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_update_float3(shaderInstance, name, (SEVector3) {
            valueX, valueY, valueZ
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_get_float3_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiShaderInstanceGetParamKWList, &shaderId, &name)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        const SEVector3 paramValue = se_shader_instance_param_get_float3(shaderInstance, name);
        return Py_BuildValue("(fff)", paramValue.x, paramValue.y, paramValue.z);
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_create_float4_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float initialValueX;
    float initialValueY;
    float initialValueZ;
    float initialValueW;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isffff", crePyApiShaderInstanceCreateFloat4ParamKWList, &shaderId, &name, &initialValueX, &initialValueY, &initialValueZ, &initialValueW)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_create_float4(shaderInstance, name, (SEVector4) {
            initialValueX, initialValueY, initialValueZ, initialValueW
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_set_float4_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    float valueX;
    float valueY;
    float valueZ;
    float valueW;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isffff", crePyApiShaderInstanceSetFloat4ParamKWList, &shaderId, &name, &valueX, &valueY, &valueZ, &valueW)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        se_shader_instance_param_update_float4(shaderInstance, name, (SEVector4) {
            valueX, valueY, valueZ, valueW
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_instance_get_float4_param(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    char* name;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiShaderInstanceGetParamKWList, &shaderId, &name)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        const SEVector4 paramValue = se_shader_instance_param_get_float4(shaderInstance, name);
        return Py_BuildValue("(ffff)", paramValue.x, paramValue.y, paramValue.z, paramValue.w);
    }
    return NULL;
}

// Shader Util
PyObject* cre_py_api_shader_util_compile_shader(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* shaderPath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiShaderUtilCompileShaderKWList, &shaderPath)) {
        const SEShaderInstanceId newId = se_shader_cache_create_instance_and_add(shaderPath);
        SE_ASSERT_FMT(newId != SE_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from path '%s'", shaderPath);
        return Py_BuildValue("i", newId);
    }
    return NULL;
}

PyObject* cre_py_api_shader_util_compile_shader_raw(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* vertexPath;
    char* fragmentPath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ss", crePyApiShaderUtilCompileShaderRawKWList, &vertexPath, &fragmentPath)) {
        const SEShaderInstanceId newId = se_shader_cache_create_instance_and_add_from_raw(vertexPath, fragmentPath);
        SE_ASSERT_FMT(newId != SE_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from paths: vertex = '%s', fragment = '%s'", vertexPath, fragmentPath);
        return Py_BuildValue("i", newId);
    }
    return NULL;
}

PyObject* cre_py_api_shader_util_set_screen_shader(PyObject* self, PyObject* args, PyObject* kwargs) {
    SEShaderInstanceId shaderId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericShaderIdKWList, &shaderId)) {
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
        if (shaderInstance != NULL) {
            se_frame_buffer_set_screen_shader(shaderInstance);
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_shader_util_reset_screen_shader_to_default(PyObject* self, PyObject* args) {
    se_frame_buffer_reset_to_default_screen_shader();
    Py_RETURN_NONE;
}

// Input
PyObject* cre_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    char* actionValue;
    int deviceId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ssi", crePyApiInputAddActionKWList, &actionName, &actionValue, &deviceId)) {
        se_input_add_action_value(actionName, actionValue, deviceId);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiInputActionInputCheckKWList, &actionName)) {
        if (se_input_is_action_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiInputActionInputCheckKWList, &actionName)) {
        if (se_input_is_action_just_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiInputActionInputCheckKWList, &actionName)) {
        if (se_input_is_action_just_released(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

// Mouse
PyObject* cre_py_api_mouse_get_position(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    const SEMouse* globalMouse = se_mouse_get();
    return Py_BuildValue("(ff)", globalMouse->position.x, globalMouse->position.y);
}

PyObject* cre_py_api_mouse_get_world_position(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    SEMouse* globalMouse = se_mouse_get();
    static SEVector2 zeroOffset = { 0.0f, 0.0f };
    const SEVector2 worldPosition = py_api_mouse_get_global_position(globalMouse, &zeroOffset);
    return Py_BuildValue("(ff)", worldPosition.x, worldPosition.y);
}

// Camera
PyObject* cre_py_api_camera2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->viewport.x = x;
        camera2D->viewport.y = y;
        cre_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->viewport.x += x;
        camera2D->viewport.y += y;
        cre_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_position(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->viewport.x, camera2D->viewport.y);
}

PyObject* cre_py_api_camera2D_set_offset(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->offset.x = x;
        camera2D->offset.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_add_to_offset(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->offset.x += x;
        camera2D->offset.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_offset(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->offset.x, camera2D->offset.y);
}

PyObject* cre_py_api_camera2D_set_zoom(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->zoom.x = x;
        camera2D->zoom.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_add_to_zoom(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->zoom.x += x;
        camera2D->zoom.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_zoom(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->zoom.x, camera2D->zoom.y);
}

// Scene Tree
PyObject* cre_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* scenePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiGenericPathKWList, &scenePath)) {
        cre_scene_manager_queue_scene_change(scenePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_scene_tree_get_root(PyObject* self, PyObject* args) {
    SceneTreeNode* rootNode = cre_scene_manager_get_active_scene_root();
    SE_ASSERT(rootNode != NULL);
    return cre_py_utils_get_entity_instance(rootNode->entity);
}

PyObject* cre_py_api_camera2D_set_boundary(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ffff", crePyApiGenericXYWHKWList, &x, &y, &w, &h)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->boundary.x = x;
        camera2D->boundary.y = y;
        camera2D->boundary.w = w;
        camera2D->boundary.h = h;
        cre_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_boundary(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ffff)", camera2D->boundary.x, camera2D->boundary.y, camera2D->boundary.w, camera2D->boundary.h);
}

PyObject* cre_py_api_camera2D_follow_node(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        cre_camera2d_follow_entity(camera2D, entity);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_unfollow_node(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        cre_camera2d_unfollow_entity(camera2D, entity);
        Py_RETURN_NONE;
    }
    return NULL;
}

// World
void py_mark_scene_nodes_time_dilation_flag_dirty(SceneTreeNode* node) {
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component_unchecked(node->entity, CreComponentDataIndex_NODE);
    SE_ASSERT(nodeComponent != NULL);
    nodeComponent->timeDilation.cacheInvalid = true;
}

PyObject* cre_py_api_world_set_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    float timeDilation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "f", crePyApiWorldSetTimeDilationKWList, &timeDilation)) {
        cre_world_set_time_dilation(timeDilation);
        cre_scene_manager_execute_on_root_and_child_nodes(py_mark_scene_nodes_time_dilation_flag_dirty);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_world_get_time_dilation(PyObject* self, PyObject* args) {
    return Py_BuildValue("f", cre_world_get_time_dilation());
}

// Audio Manager
PyObject* cre_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    bool loops;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sb", crePyApiAudioManagerPlaySoundKWList, &audioPath, &loops)) {
        se_audio_manager_play_sound(audioPath, loops);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiGenericPathKWList, &audioPath)) {
        se_audio_manager_stop_sound(audioPath);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Audio Source
PyObject* cre_py_api_audio_source_set_pitch(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    float pitch;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sf", crePyApiAudioSourceSetPitchKWList, &audioPath, &pitch)) {
        if (se_asset_manager_has_audio_source(audioPath)) {
            SEAudioSource* audioSource = se_asset_manager_get_audio_source(audioPath);
            audioSource->pitch = (double)pitch;
        } else {
            se_logger_error("Tried to set non-existent audio source's pitch at '%s'", audioPath);
        }
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_audio_source_get_pitch(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiGenericPathKWList, &audioPath)) {
        if (se_asset_manager_has_audio_source(audioPath)) {
            SEAudioSource* audioSource = se_asset_manager_get_audio_source(audioPath);
            return Py_BuildValue("f", (float)audioSource->pitch);
        }
        se_logger_error("Tried to get non-existent audio source's pitch at '%s'", audioPath);
        return Py_BuildValue("f", 1.0f);
    }
    return NULL;
}

// Game Properties
PyObject* cre_py_api_game_properties_get(PyObject* self, PyObject* args) {
    const CREGameProperties* gameProps = cre_game_props_get();
    return Py_BuildValue("(siiiiisb)",
                         gameProps->gameTitle, gameProps->resolutionWidth, gameProps->resolutionHeight,
                         gameProps->windowWidth, gameProps->windowHeight, gameProps->targetFPS,
                         gameProps->initialScenePath, gameProps->areCollidersVisible);
}

// Node
PyObject* cre_py_api_node_new(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* classPath;
    char* className;
    char* nodeType;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sss", crePyApiNodeNewKWList, &classPath, &className, &nodeType)) {
        SceneTreeNode* newNode = cre_scene_tree_create_tree_node(cre_ec_system_create_entity_uid(), NULL);

        NodeComponent* nodeComponent = node_component_create();
        strcpy(nodeComponent->name, nodeType);
        nodeComponent->type = node_get_base_type(nodeType);
        SE_ASSERT_FMT(nodeComponent->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%s'", nodeType, nodeType);
        cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_NODE, nodeComponent);

        const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(nodeComponent->type);

        if ((NodeBaseInheritanceType_NODE2D & inheritanceType) == NodeBaseInheritanceType_NODE2D) {
            Transform2DComponent* transform2DComponent = transform2d_component_create();
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_TRANSFORM_2D,
                                                transform2d_component_create());
        }
        if ((NodeBaseInheritanceType_SPRITE & inheritanceType) == NodeBaseInheritanceType_SPRITE) {
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_SPRITE, sprite_component_create());
        }
        if ((NodeBaseInheritanceType_ANIMATED_SPRITE & inheritanceType) == NodeBaseInheritanceType_ANIMATED_SPRITE) {
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_ANIMATED_SPRITE,
                                                animated_sprite_component_create());
        }
        if ((NodeBaseInheritanceType_TEXT_LABEL & inheritanceType) == NodeBaseInheritanceType_TEXT_LABEL) {
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_TEXT_LABEL,
                                                text_label_component_create());
        }
        if ((NodeBaseInheritanceType_COLLIDER2D & inheritanceType) == NodeBaseInheritanceType_COLLIDER2D) {
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_COLLIDER_2D,
                                                collider2d_component_create());
        }
        if ((NodeBaseInheritanceType_COLOR_RECT & inheritanceType) == NodeBaseInheritanceType_COLOR_RECT) {
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_COLOR_RECT,
                                                color_rect_component_create());
        }
        if ((NodeBaseInheritanceType_PARALLAX & inheritanceType) == NodeBaseInheritanceType_PARALLAX) {
            cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_PARALLAX,
                                                parallax_component_create());
        }

        cre_scene_manager_stage_child_node_to_be_added_later(newNode);

        // Setup script component
        ScriptComponent* scriptComponent = script_component_create(classPath, className);
        scriptComponent->contextType = ScriptContextType_PYTHON;
        cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_SCRIPT, scriptComponent);
        // Call create instance on script context.
        // Note: python script context checks to make sure the instance for an entity is only created once
        PyObject* entityInstance = cre_py_create_script_instance(newNode->entity, classPath, className);
        SE_ASSERT_FMT(entityInstance != NULL, "Entity instance '%d' is NULL!", newNode->entity);

        Py_IncRef(entityInstance);
        return Py_BuildValue("O", entityInstance);
    }
    return NULL;
}

PyObject* cre_py_api_node_queue_deletion(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        if (cre_scene_manager_has_entity_tree_node(entity)) {
            SceneTreeNode* node = cre_scene_manager_get_entity_tree_node(entity);
            cre_queue_destroy_tree_node_entity_all(node);
        } else {
            // TODO: May need to explicitly have an is 'being deleted' state for nodes
            se_logger_warn("Entity not found in tree, skipping queue deletion.");
        }
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_is_queued_for_deletion(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
        if (nodeComponent->queuedForDeletion) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_node_add_child(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity parentEntity;
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", crePyApiNodeAddChildKWList, &parentEntity, &entity)) {
        cre_scene_manager_add_node_as_child(entity, parentEntity);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_utils_get_entity_instance(CreEntity entity) {
#define TYPE_BUFFER_SIZE 32
    PyObject* scriptInstance = cre_py_get_script_instance(entity);
    if (scriptInstance != NULL) {
        Py_IncRef(scriptInstance);
        return scriptInstance;
    }
    // If script instance doesn't exist, create a proxy to be used on the scripting side
    char typeBuffer[TYPE_BUFFER_SIZE];
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    strcpy(typeBuffer, node_get_base_type_string(nodeComponent->type));
    return Py_BuildValue("(is)", entity, typeBuffer);
#undef TYPE_BUFFER_SIZE
}

PyObject* cre_py_api_node_get_child(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity parentEntity;
    char* childName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiNodeGetChildKWList, &parentEntity, &childName)) {
        CreEntity childEntity = cre_scene_manager_get_entity_child_by_name(parentEntity, childName);
        if (childEntity == CRE_NULL_ENTITY) {
            se_logger_warn("Failed to get child node from parent entity '%d' with the name '%s'", parentEntity,
                           childName);
            Py_RETURN_NONE;
        }
        return cre_py_utils_get_entity_instance(childEntity);
    }
    return NULL;
}

PyObject* cre_py_api_node_get_children(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity parentEntity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &parentEntity)) {
        const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(parentEntity);
        PyObject* pyChildList = PyList_New(0);
        for (size_t i = 0; i < parentTreeNode->childCount; i++) {
            const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
            PyObject* childNode = cre_py_utils_get_entity_instance(childTreeNode->entity);
            if (PyList_Append(pyChildList, childNode) == -1) {
                se_logger_error("Failed to append entity '%d' to '%d' children list!", parentEntity,
                                childTreeNode->entity);
                PyErr_Print();
            }
        }
        return pyChildList;
    }
    return NULL;
}

PyObject* cre_py_api_node_get_parent(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        if (!cre_scene_manager_has_entity_tree_node(entity)) {
            Py_RETURN_NONE;
        }
        SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(entity);
        return cre_py_utils_get_entity_instance(treeNode->parent->entity);
    }
    return NULL;
}

PyObject* cre_py_api_node_get_name(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity,
                                       CreComponentDataIndex_NODE);
        return Py_BuildValue("s", nodeComponent->name);
    }
    return NULL;
}

// TODO: Move function to another place...
void cre_invalidate_time_dilation_nodes_with_children(CreEntity entity) {
    NodeComponent* nodeComponent = cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    nodeComponent->timeDilation.cacheInvalid = true;
    SceneTreeNode* sceneTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    for (size_t i = 0; i < sceneTreeNode->childCount; i++) {
        cre_invalidate_time_dilation_nodes_with_children(sceneTreeNode->children[i]->entity);
    }
}

PyObject* cre_py_api_node_set_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float timeDilation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", crePyApiNodeSetTimeDilationKWList, &entity, &timeDilation)) {
        NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity,
                                       CreComponentDataIndex_NODE);
        nodeComponent->timeDilation.value = timeDilation;
        cre_invalidate_time_dilation_nodes_with_children(entity);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_get_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity,
                                       CreComponentDataIndex_NODE);
        return Py_BuildValue("f", nodeComponent->timeDilation.value);
    }
    return NULL;
}

PyObject* cre_py_api_node_get_full_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        return Py_BuildValue("f", cre_scene_manager_get_node_full_time_dilation(entity));
    }
    return NULL;
}

PyObject* cre_py_api_node_get_full_time_dilation_with_physics_delta(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        return Py_BuildValue("f", cre_scene_manager_get_node_full_time_dilation(entity) * CRE_GLOBAL_PHYSICS_DELTA_TIME);
    }
    return NULL;
}

// Event related stuff
void py_api_node_event_callback(void* observerData, NodeEventNotifyPayload* notifyPayload) {
    PyObject* pyCallbackFunc = (PyObject*) observerData;
    PyObject* pyEventArgs = (PyObject*) notifyPayload->data;
    SE_ASSERT(pyCallbackFunc != NULL);
    SE_ASSERT(pyEventArgs != NULL);

    PyObject* listenerFuncArg = Py_BuildValue("(O)", pyEventArgs);
    PyObject_CallObject(pyCallbackFunc, listenerFuncArg);
}

void py_api_node_event_data_delete_callback(void* data) {
    PyObject* pyCallbackFunc = (PyObject*) data;
    if (Py_IsTrue(pyCallbackFunc)) {
        Py_DecRef(pyCallbackFunc);
    }
}

PyObject* cre_py_api_node_create_event(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* eventId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiNodeCreateEventKWList, &entity, &eventId)) {
        node_event_create_event(entity, eventId);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_subscribe_to_event(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* eventId;
    CreEntity scopedEntity;
    PyObject* pCallbackFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isiO", crePyApiNodeSubscribeToEventKWList, &entity, &eventId, &scopedEntity, &pCallbackFunc)) {
        // Decreases ref in event data delete callback
        Py_IncRef(pCallbackFunc);
        node_event_subscribe_to_event(entity, eventId, scopedEntity, py_api_node_event_callback, pCallbackFunc, py_api_node_event_data_delete_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_broadcast_event(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* eventId;
    PyObject* broadcastArgs;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isO", crePyApiNodeBroadcastEventKWList, &entity, &eventId, &broadcastArgs)) {
        node_event_notify_observers(entity, eventId, &(NodeEventNotifyPayload) {
            .data = broadcastArgs
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

// Node2D
PyObject* cre_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        py_api_update_entity_local_position(entity, &(SEVector2) {
            x, y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        py_api_update_entity_local_position(entity, &(SEVector2) {
            x + transformComp->localTransform.position.x, y + transformComp->localTransform.position.y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->localTransform.position.x, transformComp->localTransform.position.y);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_global_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        SETransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transformComp);
        return Py_BuildValue("(ff)", globalTransform->position.x, globalTransform->position.y);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        py_api_update_entity_local_scale(entity, &(SEVector2) {
            x, y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_add_to_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        py_api_update_entity_local_scale(entity, &(SEVector2) {
            x + transformComp->localTransform.scale.x, y + transformComp->localTransform.scale.y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->localTransform.scale.x, transformComp->localTransform.scale.y);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float rotation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", crePyApiNode2DSetRotationKWList, &entity, &rotation)) {
        py_api_update_entity_local_rotation(entity, rotation);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_add_to_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float rotation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", crePyApiNode2DSetRotationKWList, &entity, &rotation)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        py_api_update_entity_local_rotation(entity, rotation + transformComp->localTransform.rotation);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("f", transformComp->localTransform.rotation);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_z_index(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    int zIndex;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", crePyApiNode2DSetZIndexKWList, &entity, &zIndex)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        transformComp->zIndex = zIndex;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_z_index(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("i", transformComp->zIndex);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_ignore_camera(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    bool ignoreCamera;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ib", crePyApiNode2DSetIgnoreCameraKWList, &entity, &ignoreCamera)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                                              CreComponentDataIndex_TRANSFORM_2D);
        transformComp->ignoreCamera = ignoreCamera;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_ignore_camera(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TRANSFORM_2D);
        if (transformComp->ignoreCamera) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

// Sprite
PyObject* cre_py_api_sprite_set_texture(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* filePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiSpriteSetTextureKWList, &entity, &filePath)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                                           CreComponentDataIndex_SPRITE);
        SE_ASSERT_FMT(se_asset_manager_has_texture(filePath), "Doesn't have texture with file path '%s'", filePath);
        spriteComponent->texture = se_asset_manager_get_texture(filePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_texture(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_SPRITE);
        SETexture* texture = spriteComponent->texture;
        return Py_BuildValue("(sssb)",
                             texture->fileName,
                             se_texture_get_wrap_s_string(texture),
                             se_texture_get_wrap_t_string(texture),
                             texture->applyNearestNeighbor);
    }
    return NULL;
}

PyObject* cre_py_api_sprite_set_draw_source(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float x;
    float y;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iffff", crePyApiGenericSetEntityRectKWList, &entity, &x, &y, &w, &h)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                                           CreComponentDataIndex_SPRITE);
        spriteComponent->drawSource.x = x;
        spriteComponent->drawSource.y = y;
        spriteComponent->drawSource.w = w;
        spriteComponent->drawSource.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_draw_source(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_SPRITE);
        return Py_BuildValue("(ffff)", spriteComponent->drawSource.x, spriteComponent->drawSource.y, spriteComponent->drawSource.w, spriteComponent->drawSource.h);
    }
    return NULL;
}

PyObject* cre_py_api_sprite_set_flip_h(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    bool flipH;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ib", crePyApiGenericSetEntityFlipHKWList, &entity, &flipH)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                                           CreComponentDataIndex_SPRITE);
        spriteComponent->flipH = flipH;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_flip_h(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_SPRITE);
        if (spriteComponent->flipH) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_set_flip_v(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    bool flipV;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ib", crePyApiGenericSetEntityFlipVKWList, &entity, &flipV)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                                           CreComponentDataIndex_SPRITE);
        spriteComponent->flipV = flipV;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_flip_v(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_SPRITE);
        if (spriteComponent->flipV) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_set_modulate(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    int r;
    int g;
    int b;
    int a;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", crePyApiGenericSetEntityColorKWList, &entity, &r, &g, &b, &a)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                                           CreComponentDataIndex_SPRITE);
        spriteComponent->modulate = se_color_get_normalized_color(r, g, b, a);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_modulate(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_SPRITE);
        const int red = (int) (spriteComponent->modulate.r * 255.0f);
        const int green = (int) (spriteComponent->modulate.g * 255.0f);
        const int blue = (int) (spriteComponent->modulate.b * 255.0f);
        const int alpha = (int) (spriteComponent->modulate.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

PyObject* cre_py_api_sprite_set_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    SEShaderInstanceId shaderInstanceId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", crePyApiGenericSetShaderInstanceKWList, &entity, &shaderInstanceId)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                                           CreComponentDataIndex_SPRITE);
        spriteComponent->shaderInstanceId = shaderInstanceId;
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(spriteComponent->shaderInstanceId);
        se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_SPRITE);
        const int spriteShaderInstanceId = spriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID ? (int)spriteComponent->shaderInstanceId : -1;
        return Py_BuildValue("i", spriteShaderInstanceId);
    }
    return NULL;
}

// Animated Sprite
PyObject* cre_py_api_animated_sprite_play(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* animationName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiAnimatedSpriteSetAnimationKWList, &entity, &animationName)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        const bool success = animated_sprite_component_set_animation(animatedSpriteComponent, animationName);
        animatedSpriteComponent->isPlaying = true;
        if (success) {
            animatedSpriteComponent->startAnimationTickTime = SDL_GetTicks();
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_stop(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiGenericGetEntityKWList, &entity)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->isPlaying = false;
    }
    return NULL;
}

void py_load_animated_sprite_anim_frames(CreAnimation* anim, PyObject* pyFramesList) {
    SE_ASSERT_FMT(PyList_Check(pyFramesList), "Didn't pass in a python list!");
    for (Py_ssize_t i = 0; i < PyList_Size(pyFramesList); i++) {
        PyObject* pyFrameTuple = PyList_GetItem(pyFramesList, i);
        int frame;
        char* texturePath;
        float drawSourceX;
        float drawSourceY;
        float drawSourceW;
        float drawSourceH;
        if (PyArg_ParseTuple(pyFrameTuple, "isffff", &frame, &texturePath, &drawSourceX, &drawSourceY, &drawSourceW, &drawSourceH)) {
            CreAnimationFrame animationFrame = {
                .frame = frame,
                .texture = se_asset_manager_get_texture(texturePath),
                .drawSource = { drawSourceX, drawSourceY, drawSourceW, drawSourceH }
            };
            anim->animationFrames[anim->frameCount++] = animationFrame;
        } else {
            frame = (int) i;
            se_logger_error("Failed to parse frame '%d' for anim '%s'", frame, anim->name);
        }
    }
}

PyObject* cre_py_api_animated_sprite_add_animation(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* name;
    int speed;
    bool loops;
    PyObject* framesList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isibO", crePyApiAnimatedSpriteAddAnimationKWList, &entity, &name, &speed, &loops, &framesList)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        CreAnimation newAnim = { .frameCount = 0, .currentFrame = 0, .speed = speed, .name = {'\0'}, .doesLoop = loops, .isValid = true };
        strcpy(newAnim.name, name);
        py_load_animated_sprite_anim_frames(&newAnim, framesList);
        animated_sprite_component_add_animation(animatedSpriteComponent, newAnim);
        // If the only animation set it to the current
        if (animatedSpriteComponent->animationCount == 1) {
            animated_sprite_component_set_animation(animatedSpriteComponent, newAnim.name);
        }
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_set_flip_h(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    bool flipH;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ib", crePyApiGenericSetEntityFlipHKWList, &entity, &flipH)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->flipH = flipH;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_get_flip_h(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        if (animatedSpriteComponent->flipH) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_set_flip_v(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    bool flipV;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ib", crePyApiGenericSetEntityFlipVKWList, &entity, &flipV)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->flipV = flipV;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_get_flip_v(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        if (animatedSpriteComponent->flipV) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_set_modulate(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    int r;
    int g;
    int b;
    int a;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", crePyApiGenericSetEntityColorKWList, &entity, &r, &g, &b, &a)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->modulate = se_color_get_normalized_color(r, g, b, a);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_get_modulate(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        const int red = (int) (animatedSpriteComponent->modulate.r * 255.0f);
        const int green = (int) (animatedSpriteComponent->modulate.g * 255.0f);
        const int blue = (int) (animatedSpriteComponent->modulate.b * 255.0f);
        const int alpha = (int) (animatedSpriteComponent->modulate.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_set_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    SEShaderInstanceId shaderInstanceId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", crePyApiGenericSetShaderInstanceKWList, &entity, &shaderInstanceId)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->shaderInstanceId = shaderInstanceId;
        SEShaderInstance* shaderInstance = se_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
        se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_get_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_ANIMATED_SPRITE);
        const int animatedSpriteShaderInstanceId = animatedSpriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID ? (int)animatedSpriteComponent->shaderInstanceId : -1;
        return Py_BuildValue("i", animatedSpriteShaderInstanceId);
    }
    return NULL;
}

// Text Label
PyObject* cre_py_api_text_label_set_text(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* text;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiTextLabelSetTextKWList, &entity, &text)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TEXT_LABEL);
        strcpy(textLabelComponent->text, text);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_text_label_get_text(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TEXT_LABEL);
        return Py_BuildValue("s", textLabelComponent->text);
    }
    return NULL;
}

PyObject* cre_py_api_text_label_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iffff", crePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TEXT_LABEL);
        textLabelComponent->color = se_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_text_label_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TEXT_LABEL);
        const int red = (int) (textLabelComponent->color.r * 255.0f);
        const int green = (int) (textLabelComponent->color.g * 255.0f);
        const int blue = (int) (textLabelComponent->color.b * 255.0f);
        const int alpha = (int) (textLabelComponent->color.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

PyObject* cre_py_api_text_label_set_font_uid(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    char* uid;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiTextLabelSetFontUIDKWList, &entity, &uid)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TEXT_LABEL);
        if (se_asset_manager_has_font(uid)) {
            textLabelComponent->font = se_asset_manager_get_font(uid);
        } else {
            se_logger_error("Failed to set font to '%s' as it doesn't exist in the asset manager!", uid);
        }
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_text_label_get_font_uid(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_TEXT_LABEL);
        return Py_BuildValue("s", "default"); // TODO: Do want this?
    }
    return NULL;
}

// Collider2D
PyObject* cre_py_api_collider2D_set_extents(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiGenericSetEntitySize2DKWList, &entity, &w, &h)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLLIDER_2D);
        collider2DComponent->extents.w = w;
        collider2DComponent->extents.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_collider2D_get_extents(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Collider2DComponent* collider2DComponent = (Collider2DComponent *) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_COLLIDER_2D);
        return Py_BuildValue("(ff)", collider2DComponent->extents.w, collider2DComponent->extents.h);
    }
    return NULL;
}

PyObject* cre_py_api_collider2D_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", crePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLLIDER_2D);
        collider2DComponent->color = se_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_collider2D_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent *) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLLIDER_2D);
        const int red = (int) (collider2DComponent->color.r * 255.0f);
        const int green = (int) (collider2DComponent->color.g * 255.0f);
        const int blue = (int) (collider2DComponent->color.b * 255.0f);
        const int alpha = (int) (collider2DComponent->color.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

// ColorRect
PyObject* cre_py_api_color_rect_set_size(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiGenericSetEntitySize2DKWList, &entity, &w, &h)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLOR_RECT);
        colorSquareComponent->size.w = w;
        colorSquareComponent->size.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_color_rect_get_size(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const ColorRectComponent* colorSquareComponent = (ColorRectComponent*) cre_component_manager_get_component(
                    entity, CreComponentDataIndex_COLOR_RECT);
        return Py_BuildValue("(ff)", colorSquareComponent->size.w, colorSquareComponent->size.h);
    }
    return NULL;
}

PyObject* cre_py_api_color_rect_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", crePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent *) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLOR_RECT);
        colorSquareComponent->color = se_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_color_rect_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent*) cre_component_manager_get_component(entity,
                CreComponentDataIndex_COLOR_RECT);
        const int red = (int) (colorSquareComponent->color.r * 255.0f);
        const int green = (int) (colorSquareComponent->color.g * 255.0f);
        const int blue = (int) (colorSquareComponent->color.b * 255.0f);
        const int alpha = (int) (colorSquareComponent->color.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

// Network
PyObject* cre_py_api_network_is_server(PyObject* self, PyObject* args) {
    if (se_network_is_server()) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Server
PyObject* cre_py_api_server_start(PyObject* self, PyObject* args, PyObject* kwargs) {
    int port;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiServerStartKWList, &port)) {
        se_udp_server_initialize(port, cre_ec_system_network_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_server_stop(PyObject* self, PyObject* args) {
    se_udp_server_finalize();
    Py_RETURN_NONE;
}

PyObject* cre_py_api_server_send(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* message;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiNetworkSendMessageKWList, &message)) {
        se_udp_server_send_message(message);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_server_subscribe(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* signalId;
    CreEntity listenerNode;
    PyObject* listenerFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siO", crePyApiNetworkSubscribeKWList, &signalId, &listenerNode, &listenerFunc)) {
        SE_ASSERT(PyObject_IsTrue(listenerFunc));
        const CREScriptContext* scriptContext = cre_py_get_script_context();
        SE_ASSERT(scriptContext != NULL && scriptContext->on_entity_subscribe_to_network_callback != NULL);
        scriptContext->on_entity_subscribe_to_network_callback(listenerNode, listenerFunc, signalId);

        Py_DECREF(listenerFunc);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Client
PyObject* cre_py_api_client_start(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* host;
    int port;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "si", crePyApiClientStartKWList, &host, &port)) {
        se_udp_client_initialize(host, port, cre_ec_system_network_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_client_stop(PyObject* self, PyObject* args) {
    se_udp_client_finalize();
    Py_RETURN_NONE;
}

PyObject* cre_py_api_client_send(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* message;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiNetworkSendMessageKWList, &message)) {
        se_udp_client_send_message(message);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_client_subscribe(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* signalId;
    CreEntity listenerNode;
    PyObject* listenerFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siO", crePyApiNetworkSubscribeKWList, &signalId, &listenerNode, &listenerFunc)) {
        SE_ASSERT(PyObject_IsTrue(listenerFunc));
        const CREScriptContext* scriptContext = cre_py_get_script_context();
        SE_ASSERT(scriptContext != NULL && scriptContext->on_entity_subscribe_to_network_callback != NULL);
        scriptContext->on_entity_subscribe_to_network_callback(listenerNode, listenerFunc, signalId);

        Py_DECREF(listenerFunc);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Collision Handler
PyObject* cre_py_api_collision_handler_process_collisions(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreEntity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        PyObject* pyCollidedEntityList = PyList_New(0);
        CollisionResult collisionResult = cre_collision_process_entity_collisions(entity);
        for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
            const CreEntity collidedEntity = collisionResult.collidedEntities[i];
            PyObject* collidedNode = cre_py_utils_get_entity_instance(collidedEntity);
            if (PyList_Append(pyCollidedEntityList, collidedNode) == -1) {
                se_logger_error("Failed to append collided entity '%d' to collision list!", collidedEntity);
                PyErr_Print();
            }
        }
        return Py_BuildValue("O", pyCollidedEntityList);
    }
    return NULL;
}

PyObject* cre_py_api_collision_handler_process_mouse_collisions(PyObject* self, PyObject* args, PyObject* kwargs) {
    float positionOffsetX;
    float positionOffsetY;
    float collisionSizeW;
    float collisionSizeH;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ffff", crePyApiCollisionHandlerProcessMouseCollisionsKWList, &positionOffsetX, &positionOffsetY, &collisionSizeW, &collisionSizeH)) {
        PyObject* pyCollidedEntityList = PyList_New(0);
        // TODO: Transform mouse screen position into world position.
        SEMouse* globalMouse = se_mouse_get();
        SEVector2 positionOffset = { positionOffsetX, positionOffsetY };
        const SEVector2 mouseWorldPos = py_api_mouse_get_global_position(globalMouse, &positionOffset);
        SERect2 collisionRect = { mouseWorldPos.x, mouseWorldPos.y, collisionSizeW, collisionSizeH };
        CollisionResult collisionResult = cre_collision_process_mouse_collisions(&collisionRect);
        for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
            const CreEntity collidedEntity = collisionResult.collidedEntities[i];
            PyObject* collidedNode = cre_py_utils_get_entity_instance(collidedEntity);
            if (PyList_Append(pyCollidedEntityList, collidedNode) == -1) {
                se_logger_error("Failed to append mouse collided entity '%d' to collision list!", collidedEntity);
                PyErr_Print();
            }
        }
        return Py_BuildValue("O", pyCollidedEntityList);
    }
    return NULL;
}

// Game Config
PyObject* cre_py_api_game_config_save(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* path;
    char* dataJson;
    char* encryptionKey;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sss", crePyApiGameConfigSaveKWList, &path, &dataJson, &encryptionKey)) {
        CREGameProperties* gameProps = cre_game_props_get();
        char* validGameTitle = se_strdup(gameProps->gameTitle);
        se_str_to_lower_and_underscore_whitespace(validGameTitle);
        char* fullSavePath = se_fs_get_user_save_path("crescent", validGameTitle, path);
        const bool success = se_fs_write_to_file(fullSavePath, dataJson);
        SE_MEM_FREE(validGameTitle);
        SE_MEM_FREE(fullSavePath);
        if (success) {
            Py_RETURN_TRUE;
        }
    }
    Py_RETURN_FALSE;
}

PyObject* cre_py_api_game_config_load(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* path;
    char* encryptionKey;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ss", crePyApiGameConfigLoadKWList, &path, &encryptionKey)) {
        CREGameProperties* gameProps = cre_game_props_get();
        char* validGameTitle = se_strdup(gameProps->gameTitle);
        se_str_to_lower_and_underscore_whitespace(validGameTitle);
        char* fullSavePath = se_fs_get_user_save_path("crescent", validGameTitle, path);
        char* fileContents = sf_asset_file_loader_read_file_contents_as_string(fullSavePath, NULL);
        PyObject* returnValue =  Py_BuildValue("s", fileContents);
        SE_MEM_FREE(validGameTitle);
        SE_MEM_FREE(fullSavePath);
        SE_MEM_FREE(fileContents);
        return returnValue;
    }
    return Py_BuildValue("s", "{}");
}

// Packed Scene
PyObject* cre_py_api_packed_scene_create_instance(PyObject* self, PyObject* args, PyObject* kwargs) {
    CreSceneCacheId cacheId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiPackedSceneCreateInstanceKWList, &cacheId)) {
        JsonSceneNode* sceneNode = cre_scene_template_cache_get_scene(cacheId);
        SceneTreeNode* rootNode = cre_scene_manager_stage_scene_nodes_from_json(sceneNode);
        ScriptComponent* scriptComponent = (ScriptComponent*) cre_component_manager_get_component_unchecked(
                                               rootNode->entity, CreComponentDataIndex_SCRIPT);
        if (!scriptComponent) {
            // Create new script component if it doesn't exist
            scriptComponent = script_component_create("crescent_api", node_get_base_type_string(sceneNode->type));
            scriptComponent->contextType = ScriptContextType_PYTHON;
            cre_component_manager_set_component(rootNode->entity, CreComponentDataIndex_SCRIPT, scriptComponent);
        }
        PyObject* entityInstance = cre_py_create_script_instance(rootNode->entity, scriptComponent->classPath, scriptComponent->className);

        Py_IncRef(entityInstance);
        return Py_BuildValue("O", entityInstance);
    }
    return NULL;
}

// Scene Util
PyObject* cre_py_api_scene_util_load_scene(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* scenePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiGenericPathKWList, &scenePath)) {
        CreSceneCacheId cacheId = CRE_SCENE_CACHE_INVALID_ID;
        char* sceneText = sf_asset_file_loader_read_file_contents_as_string(scenePath, NULL);
        if (sceneText) {
            cacheId = cre_scene_template_cache_load_scene(scenePath);
        }
        return Py_BuildValue("i", cacheId);
    }
    return NULL;
}
