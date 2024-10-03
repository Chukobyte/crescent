#include "pkpy_api_impl.h"

#include <seika/assert.h>
#include <seika/file_system.h>
#include <seika/logger.h>
#include <seika/memory.h>
#include <seika/input/input.h>
#include <seika/asset/asset_file_loader.h>
#include <seika/asset/asset_manager.h>
#include <seika/audio/audio.h>
#include <seika/audio/audio_manager.h>
#include <seika/ecs/ec_system.h>
#include <seika/networking/network.h>
#include <seika/rendering/render_context.h>
#include <seika/rendering/frame_buffer.h>
#include <seika/rendering/shader/shader_instance_minimal.h>
#include <seika/rendering/shader/shader_instance.h>
#include <seika/rendering/shader/shader_cache.h>

#include "core/engine_context.h"
#include "core/game_properties.h"
#include "core/world.h"
#include "core/camera/camera.h"
#include "core/camera/camera_manager.h"
#include "core/ecs/ecs_globals.h"
#include "core/ecs/ecs_manager.h"
#include "core/ecs/components/animated_sprite_component.h"
#include "core/ecs/components/color_rect_component.h"
#include "core/ecs/components/parallax_component.h"
#include "core/ecs/components/particles2d_component.h"
#include "core/ecs/components/script_component.h"
#include "core/ecs/components/sprite_component.h"
#include "core/ecs/components/text_label_component.h"
#include "core/ecs/components/tilemap_component.h"
#include "core/physics/collision/collision.h"
#include "core/scene/scene_manager.h"
#include "core/scene/scene_template_cache.h"
#include "core/scripting/python/pocketpy/pkpy_instance_cache.h"
#include "seika/flag_utils.h"

// Helper functions
static inline SkaVector2 cre_pkpy_api_helper_mouse_get_global_position(const SkaVector2* offset) {
    SkaMouse* globalMouse = ska_input_get_mouse();
    const CRECamera2D* camera = cre_camera_manager_get_current_camera();
    CREGameProperties* gameProps = cre_game_props_get();
    SkaRenderContext* renderContext = ska_render_context_get();
    const SkaVector2 mouse_pixel_coord = {
        ska_math_map_to_range(globalMouse->position.x, 0.0f, (float) renderContext->windowWidth, 0.0f, (float) gameProps->resolutionWidth),
        ska_math_map_to_range(globalMouse->position.y, 0.0f, (float) renderContext->windowHeight, 0.0f, (float) gameProps->resolutionHeight)
};
    const SkaVector2 mouseWorldPos = {
        (camera->viewport.x + camera->offset.x + mouse_pixel_coord.x + offset->x) * camera->zoom.x,
        (camera->viewport.y + camera->offset.y + mouse_pixel_coord.y + offset->y) * camera->zoom.y
};
    return mouseWorldPos;
}

// TODO: Check parameter types to ensure the correct parameters are passed into the api functions

// Shader Instance

bool cre_pkpy_api_shader_instance_delete(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyShaderId = py_toint(py_arg(0));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    bool hasDeletedInstance = false;
    if (shaderInstance) {
        ska_shader_cache_remove_instance(shaderId);
        ska_shader_instance_destroy(shaderInstance);
    }
    py_newbool(py_retval(), hasDeletedInstance);
    return true;
}

bool cre_pkpy_api_shader_instance_create_bool_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_bool);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const bool value = py_tobool(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_create_bool(shaderInstance, paramName, value);
    return true;
}

bool cre_pkpy_api_shader_instance_set_bool_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_bool);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const bool value = py_tobool(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_update_bool(shaderInstance, paramName, value);
    return true;
}

bool cre_pkpy_api_shader_instance_get_bool_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const bool value = ska_shader_instance_param_get_bool(shaderInstance, paramName);
    py_newbool(py_retval(), value);
    return true;
}

bool cre_pkpy_api_shader_instance_create_int_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_int);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const py_i64 value = py_toint(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_create_int(shaderInstance, paramName, (int32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_set_int_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_int);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const py_i64 value = py_toint(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_update_int(shaderInstance, paramName, (int32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_get_int_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const int32 value = ska_shader_instance_param_get_int(shaderInstance, paramName);
    py_newint(py_retval(), value);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 value = py_tofloat(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_create_float(shaderInstance, paramName, (f32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 value = py_tofloat(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_update_float(shaderInstance, paramName, (f32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const f32 value = ska_shader_instance_param_get_float(shaderInstance, paramName);
    py_newfloat(py_retval(), value);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float2_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector2 vecValue = (SkaVector2){ .x = (f32)valueX, .y = (f32)valueY };
    ska_shader_instance_param_create_float2(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float2_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector2 vecValue = (SkaVector2){ .x = (f32)valueX, .y = (f32)valueY };
    ska_shader_instance_param_update_float2(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float2_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector2 value = ska_shader_instance_param_get_float2(shaderInstance, paramName);
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, value.x);
    py_newfloat(pyY, value.y);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float3_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(5);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float); PY_CHECK_ARG_TYPE(4, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector3 vecValue = (SkaVector3){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ };
    ska_shader_instance_param_create_float3(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float3_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(5);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float); PY_CHECK_ARG_TYPE(4, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector3 vecValue = (SkaVector3){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ };
    ska_shader_instance_param_update_float3(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float3_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector3 value = ska_shader_instance_param_get_float3(shaderInstance, paramName);
    py_newtuple(py_retval(), 3);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_Ref pyZ = py_tuple_getitem(py_retval(), 2);
    py_newfloat(pyX, value.x);
    py_newfloat(pyY, value.y);
    py_newfloat(pyZ, value.z);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float4_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(6);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float); PY_CHECK_ARG_TYPE(4, tp_float); PY_CHECK_ARG_TYPE(5, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));
    const f64 valueW = py_tofloat(py_arg(5));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector4 vecValue = (SkaVector4){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ, .w = (f32)valueW };
    ska_shader_instance_param_create_float4(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float4_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(6);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float); PY_CHECK_ARG_TYPE(4, tp_float); PY_CHECK_ARG_TYPE(5, tp_float);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));
    const f64 valueW = py_tofloat(py_arg(5));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector4 vecValue = (SkaVector4){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ, .w = (f32)valueW };
    ska_shader_instance_param_update_float4(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float4_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector4 value = ska_shader_instance_param_get_float4(shaderInstance, paramName);
    py_newtuple(py_retval(), 4);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_Ref pyZ = py_tuple_getitem(py_retval(), 2);
    py_Ref pyW = py_tuple_getitem(py_retval(), 3);
    py_newfloat(pyX, value.x);
    py_newfloat(pyY, value.y);
    py_newfloat(pyZ, value.z);
    py_newfloat(pyW, value.w);
    return true;
}

// Shader Util

bool cre_pkpy_api_shader_util_compile_shader(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* shaderPath = py_tostr(py_arg(0));

    const SkaShaderInstanceId newId = ska_shader_cache_create_instance_and_add(shaderPath);
    SKA_ASSERT_FMT(newId != SKA_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from path '%s'", shaderPath);
    py_newint(py_retval(), newId);
    return true;
}

bool cre_pkpy_api_shader_util_compile_shader_raw(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_str);
    const char* vertexPath = py_tostr(py_arg(0));
    const char* fragmentPath = py_tostr(py_arg(1));

    const SkaShaderInstanceId newId = ska_shader_cache_create_instance_and_add_from_raw(vertexPath, fragmentPath);
    SKA_ASSERT_FMT(newId != SKA_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from paths: vertex = '%s', fragment = '%s'", vertexPath, fragmentPath);
    py_newint(py_retval(), newId);
    return true;
}

bool cre_pkpy_api_shader_util_set_screen_shader(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyShaderId = py_toint(py_arg(0));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    bool hasSetShaderInstance = false;
    if (shaderInstance) {
        ska_frame_buffer_set_screen_shader(shaderInstance);
        hasSetShaderInstance = true;
    }
    py_newbool(py_retval(), hasSetShaderInstance);
    return true;
}

// TODO: Keep track of current screen shader
bool cre_pkpy_api_shader_util_get_current_screen_shader(int argc, py_StackRef argv) {
    py_newint(py_retval(), 0);
    return true;
}

bool cre_pkpy_api_shader_util_reset_screen_shader_to_default(int argc, py_StackRef argv) {
    ska_frame_buffer_reset_to_default_screen_shader();
    return true;
}

// Engine

bool cre_pkpy_api_engine_exit(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyExitCode = py_toint(py_arg(0));

    CREEngineContext* engineContext = cre_engine_context_get();
    engineContext->isRunning = false;
    engineContext->exitCode = (int32)pyExitCode;
    return true;
}

bool cre_pkpy_api_engine_set_target_fps(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyTargetFPS = py_toint(py_arg(0));

    CREEngineContext* engineContext = cre_engine_context_get();
    engineContext->targetFPS = (int32)pyTargetFPS;
    return true;
}

bool cre_pkpy_api_engine_get_target_fps(int argc, py_StackRef argv) {
    CREEngineContext* engineContext = cre_engine_context_get();
    py_newint(py_retval(), engineContext->targetFPS);
    return true;
}

bool cre_pkpy_api_engine_get_average_fps(int argc, py_StackRef argv) {
    CREEngineContext* engineContext = cre_engine_context_get();
    py_newint(py_retval(), (py_i64)engineContext->stats.averageFPS);
    return true;
}

bool cre_pkpy_api_engine_set_fps_display_enabled(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_bool); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float);
    const bool isEnabled = py_tobool(py_arg(0));
    const char* fontUID = py_tostr(py_arg(1));
    const py_f64 pyPosX = py_tofloat(py_arg(2));
    const py_f64 pyPosY = py_tofloat(py_arg(3));

    cre_ecs_manager_enable_fps_display_entity(isEnabled, fontUID, (f32)pyPosX, (f32)pyPosY);
    return true;
}

bool cre_pkpy_api_engine_get_global_physics_delta_time(int argc, py_StackRef argv) {
    py_newfloat(py_retval(), CRE_GLOBAL_PHYSICS_DELTA_TIME);
    return true;
}

// Input

bool cre_pkpy_api_input_is_key_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyKey = py_toint(py_arg(0));

    const bool isPressed = ska_input_is_key_pressed((SkaInputKey)pyKey, SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX);
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_key_just_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyKey = py_toint(py_arg(0));

    const bool isPressed = ska_input_is_key_just_pressed((SkaInputKey)pyKey, SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX);
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_key_just_released(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyKey = py_toint(py_arg(0));

    const bool isPressed = ska_input_is_key_just_released((SkaInputKey)pyKey, SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX);
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_add_action(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_int); PY_CHECK_ARG_TYPE(2, tp_int);
    const char* actionName = py_tostr(py_arg(0));
    const py_i64 pyValueKey = py_toint(py_arg(1));
    const py_i64 pyDeviceId = py_toint(py_arg(2));

    ska_input_add_input_action(
        actionName,
        (SkaInputActionValue[]){ { .key = (SkaInputKey)pyValueKey, .strengthThreshold = 0.5f }, { SkaInputKey_INVALID } },
        (SkaInputDeviceIndex)pyDeviceId
    );
    return true;
}

bool cre_pkpy_api_input_is_action_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* actionName = py_tostr(py_arg(0));

    // TODO: Probably should take device index as a param
    const SkaInputDeviceIndex deviceIndex = SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX;
    const SkaInputActionHandle handle = ska_input_find_input_action_handle(actionName, deviceIndex);
    const bool isPressed = handle != SKA_INPUT_INVALID_INPUT_ACTION_HANDLE ? ska_input_is_input_action_pressed(handle, deviceIndex) : false;
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_action_just_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* actionName = py_tostr(py_arg(0));

    const SkaInputDeviceIndex deviceIndex = SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX;
    const SkaInputActionHandle handle = ska_input_find_input_action_handle(actionName, deviceIndex);
    const bool isPressed = handle != SKA_INPUT_INVALID_INPUT_ACTION_HANDLE ? ska_input_is_input_action_just_pressed(handle, deviceIndex) : false;
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_action_just_released(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* actionName = py_tostr(py_arg(0));

    const SkaInputDeviceIndex deviceIndex = SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX;
    const SkaInputActionHandle handle = ska_input_find_input_action_handle(actionName, deviceIndex);
    const bool isReleased = handle != SKA_INPUT_INVALID_INPUT_ACTION_HANDLE ? ska_input_is_input_action_just_released(handle, deviceIndex) : false;
    py_newbool(py_retval(), isReleased);
    return true;
}

// TODO: Finish implementing
bool cre_pkpy_api_input_start_gamepad_vibration(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float);
    const py_i64 pyDeviceId = py_toint(py_arg(0));
    const py_f64 pyWeakMagnitude = py_tofloat(py_arg(1));
    const py_f64 pyStrongMagnitude = py_tofloat(py_arg(2));
    const py_f64 pyDurationSeconds = py_tofloat(py_arg(3));

    // ska_input_gamepad_start_vibration((int32)pyDeviceId, (f32)pyWeakMagnitude, (f32)pyStrongMagnitude, (f32)pyDurationSeconds);
    return true;
}

// TODO: Finish implementing
bool cre_pkpy_api_input_stop_gamepad_vibration(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 pyDeviceId = py_toint(py_arg(0));

    // ska_input_gamepad_stop_vibration((SkaInputDeviceIndex)pyDeviceId);
    return true;
}

bool cre_pkpy_api_input_mouse_get_position(int argc, py_StackRef argv) {
    const SkaMouse* globalMouse = ska_input_get_mouse();
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, (py_f64)globalMouse->position.x);
    py_newfloat(pyY, (py_f64)globalMouse->position.y);
    return true;
}

bool cre_pkpy_api_input_mouse_get_world_position(int argc, py_StackRef argv) {
    const SkaVector2 mouseWorldPosition = cre_pkpy_api_helper_mouse_get_global_position(&SKA_VECTOR2_ZERO);
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, (py_f64)mouseWorldPosition.x);
    py_newfloat(pyY, (py_f64)mouseWorldPosition.y);
    return true;
}

// Scene Tree

bool cre_pkpy_api_scene_tree_change_scene(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* scenePath = py_tostr(py_arg(0));

    cre_scene_manager_queue_scene_change(scenePath);
    return true;
}

bool cre_pkpy_api_scene_tree_get_root(int argc, py_StackRef argv) {
    SceneTreeNode* rootNode = cre_scene_manager_get_active_scene_root();
    SKA_ASSERT(rootNode != NULL);
    py_Ref rootInstance = cre_pkpy_instance_cache_add2(rootNode->entity);
    py_assign(py_retval(), rootInstance);
    return true;
}

// Scene Manager

bool cre_pkpy_api_scene_manager_process_queued_creation_entities(int argc, py_StackRef argv) {
    cre_scene_manager_process_queued_creation_entities();
    return true;
}

bool cre_pkpy_api_scene_manager_process_queued_scene_change(int argc, py_StackRef argv) {
    cre_scene_manager_process_queued_scene_change();
    return true;
}

// Game Properties

bool cre_pkpy_api_game_properties_get(int argc, py_StackRef argv) {
    const CREGameProperties* gameProps = cre_game_props_get();
    SKA_ASSERT(gameProps->gameTitle);
    SKA_ASSERT(gameProps->initialScenePath);
    py_newtuple(py_retval(), 8);
    py_Ref pyGameTitle = py_tuple_getitem(py_retval(), 0);
    py_Ref pyResolutionWidth = py_tuple_getitem(py_retval(), 1);
    py_Ref pyResolutionHeight = py_tuple_getitem(py_retval(), 2);
    py_Ref pyWindowWidth = py_tuple_getitem(py_retval(), 3);
    py_Ref pyWindowHeight = py_tuple_getitem(py_retval(), 4);
    py_Ref pyTargetFPS = py_tuple_getitem(py_retval(), 5);
    py_Ref pyInitialScenePath = py_tuple_getitem(py_retval(), 6);
    py_Ref pyAreColliderVisible = py_tuple_getitem(py_retval(), 7);

    py_newstr(pyGameTitle, gameProps->gameTitle);
    py_newint(pyResolutionWidth, gameProps->resolutionWidth);
    py_newint(pyResolutionHeight, gameProps->resolutionHeight);
    py_newint(pyWindowWidth, gameProps->windowWidth);
    py_newint(pyWindowHeight, gameProps->windowHeight);
    py_newint(pyTargetFPS, gameProps->targetFPS);
    py_newstr(pyInitialScenePath, gameProps->initialScenePath);
    py_newbool(pyAreColliderVisible, gameProps->areCollidersVisible);
    return true;
}

// Camera2D

bool cre_pkpy_api_camera2d_set_position(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float);
    const f64 posX = py_tofloat(py_arg(0));
    const f64 posY = py_tofloat(py_arg(1));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->viewport = (SkaVector2){ .x = (f32)posX, .y = (f32)posY };
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
    return true;
}

bool cre_pkpy_api_camera2d_add_to_position(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float);
    const f64 posX = py_tofloat(py_arg(0));
    const f64 posY = py_tofloat(py_arg(1));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->viewport = (SkaVector2){ .x = camera2D->viewport.x + (f32)posX, .y = camera2D->viewport.y + (f32)posY };
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
    return true;
}

bool cre_pkpy_api_camera2d_get_position(int argc, py_StackRef argv) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, camera2D->viewport.x);
    py_newfloat(pyY, camera2D->viewport.y);
    return true;
}

bool cre_pkpy_api_camera2d_set_offset(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float);
    const f64 offsetX = py_tofloat(py_arg(0));
    const f64 offsetY = py_tofloat(py_arg(1));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->offset = (SkaVector2){ .x = (f32)offsetX, .y = (f32)offsetY };
    return true;
}

bool cre_pkpy_api_camera2d_add_to_offset(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float);
    const f64 offsetX = py_tofloat(py_arg(0));
    const f64 offsetY = py_tofloat(py_arg(1));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->offset = (SkaVector2){ .x = camera2D->offset.x + (f32)offsetX, .y = camera2D->offset.y + (f32)offsetY };
    return true;
}

bool cre_pkpy_api_camera2d_get_offset(int argc, py_StackRef argv) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, camera2D->offset.x);
    py_newfloat(pyY, camera2D->offset.y);
    return true;
}

bool cre_pkpy_api_camera2d_set_zoom(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float);
    const f64 zoomX = py_tofloat(py_arg(0));
    const f64 zoomY = py_tofloat(py_arg(1));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->zoom = (SkaVector2){ .x = (f32)zoomX, .y = (f32)zoomY };
    return true;
}

bool cre_pkpy_api_camera2d_add_to_zoom(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float);
    const f64 zoomX = py_tofloat(py_arg(0));
    const f64 zoomY = py_tofloat(py_arg(1));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->zoom = (SkaVector2){ .x = camera2D->zoom.x + (f32)zoomX, .y = camera2D->zoom.y + (f32)zoomY };
    return true;
}

bool cre_pkpy_api_camera2d_get_zoom(int argc, py_StackRef argv) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, camera2D->zoom.x);
    py_newfloat(pyY, camera2D->zoom.y);
    return true;
}

bool cre_pkpy_api_camera2d_set_boundary(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float);
    const f64 x = py_tofloat(py_arg(0));
    const f64 y = py_tofloat(py_arg(1));
    const f64 w = py_tofloat(py_arg(2));
    const f64 h = py_tofloat(py_arg(3));

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->boundary = (SkaRect2){ .x = (f32)x, .y = (f32)y, .w = (f32)w, .h = (f32)h };
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
    return true;
}

bool cre_pkpy_api_camera2d_get_boundary(int argc, py_StackRef argv) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    py_newtuple(py_retval(), 4);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_Ref pyW = py_tuple_getitem(py_retval(), 2);
    py_Ref pyH = py_tuple_getitem(py_retval(), 3);
    py_newfloat(pyX, camera2D->boundary.x);
    py_newfloat(pyY, camera2D->boundary.y);
    py_newfloat(pyW, camera2D->boundary.w);
    py_newfloat(pyH, camera2D->boundary.h);
    return true;
}

bool cre_pkpy_api_camera2d_follow_node(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    cre_camera2d_follow_entity(camera2D, entity);
    return true;
}

bool cre_pkpy_api_camera2d_unfollow_node(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    cre_camera2d_unfollow_entity(camera2D, entity);
    return true;
}

// World

static void mark_scene_nodes_time_dilation_flag_dirty(SceneTreeNode* node) {
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(node->entity, NODE_COMPONENT_INDEX);
    nodeComponent->timeDilation.cacheInvalid = true;
}

bool cre_pkpy_api_world_set_time_dilation(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_float);
    const f64 timeDilation = py_tofloat(py_arg(0));

    cre_world_set_time_dilation((f32)timeDilation);
    cre_scene_manager_execute_on_root_and_child_nodes(mark_scene_nodes_time_dilation_flag_dirty);
    return true;
}

bool cre_pkpy_api_world_get_time_dilation(int argc, py_StackRef argv) {
    py_newfloat(py_retval(), (f64)cre_world_get_time_dilation());
    return true;
}

bool cre_pkpy_api_world_get_delta_time(int argc, py_StackRef argv) {
    py_newfloat(py_retval(), (f64)(cre_world_get_time_dilation() * CRE_GLOBAL_PHYSICS_DELTA_TIME));
    return true;
}

bool cre_pkpy_api_world_get_variable_delta_time(int argc, py_StackRef argv) {
    py_newfloat(py_retval(), (f64)cre_world_get_frame_delta_time());
    return true;
}

// Audio Source

bool cre_pkpy_api_audio_source_set_pitch(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_float);
    const char* path = py_tostr(py_arg(0));
    const f64 pitch = py_tofloat(py_arg(1));

    if (ska_asset_manager_has_audio_source(path)) {
        SkaAudioSource* audioSource = ska_asset_manager_get_audio_source(path);
        audioSource->pitch = pitch;
    } else {
        ska_logger_warn("Tried to set non-existent audio source's pitch at '%s'", path);
    }
    return true;
}

bool cre_pkpy_api_audio_source_get_pitch(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* path = py_tostr(py_arg(0));

    f64 pitch;
    if (ska_asset_manager_has_audio_source(path)) {
        SkaAudioSource* audioSource = ska_asset_manager_get_audio_source(path);
        pitch = (f64)audioSource->pitch;
    } else {
        ska_logger_warn("Tried to get non-existent audio source's pitch at '%s'", path);
        pitch = 1.0;
    }
    py_newfloat(py_retval(), pitch);
    return true;
}

// Audio Manager

bool cre_pkpy_api_audio_manager_play_sound(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_bool);
    const char* path = py_tostr(py_arg(0));
    const bool loops = py_tobool(py_arg(1));

    ska_audio_manager_play_sound(path, loops);
    return true;
}

bool cre_pkpy_api_audio_manager_stop_sound(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* path = py_tostr(py_arg(0));

    ska_audio_manager_stop_sound(path);
    return true;
}

// Game Config

bool cre_pkpy_api_game_config_save(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_str);
    const char* path = py_tostr(py_arg(0));
    const char* jsonData = py_tostr(py_arg(1));
    const char* encryptionKey = py_tostr(py_arg(2));

    const CREGameProperties* gameProps = cre_game_props_get();
    char* validGameTitle = ska_strdup(gameProps->gameTitle);
    ska_str_to_lower_and_underscore_whitespace(validGameTitle);
    char* fullSavePath = ska_fs_get_user_save_path("crescent", validGameTitle, path);
    const bool success = ska_fs_write_to_file(fullSavePath, jsonData);
    SKA_MEM_FREE(fullSavePath);
    SKA_MEM_FREE(validGameTitle);
    py_newbool(py_retval(), success);
    return true;
}

bool cre_pkpy_api_game_config_load(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_str);
    const char* path = py_tostr(py_arg(0));
    const char* encryptionKey = py_tostr(py_arg(1));

    const CREGameProperties* gameProps = cre_game_props_get();
    char* validGameTitle = ska_strdup(gameProps->gameTitle);
    ska_str_to_lower_and_underscore_whitespace(validGameTitle);
    char* fullSavePath = ska_fs_get_user_save_path("crescent", validGameTitle, path);
    char* fileContents = ska_asset_file_loader_read_file_contents_as_string(fullSavePath, NULL);
    py_newstr(py_retval(), fileContents);
    SKA_MEM_FREE(validGameTitle);
    SKA_MEM_FREE(fullSavePath);
    SKA_MEM_FREE(fileContents);
    return true;
}

// Packed Scene

bool cre_pkpy_api_packed_scene_create_instance(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 cacheId = py_toint(py_arg(0));

    JsonSceneNode* sceneNode = cre_scene_template_cache_get_scene((CreSceneCacheId)cacheId);
    SceneTreeNode* rootNode = cre_scene_manager_stage_scene_nodes_from_json(sceneNode);

    py_Ref newInstance;
    if (sceneNode->components[SCRIPT_COMPONENT_INDEX] != NULL) {
        ScriptComponent* scriptComp = (ScriptComponent*)sceneNode->components[SCRIPT_COMPONENT_INDEX];
        newInstance = cre_pkpy_instance_cache_add(rootNode->entity, scriptComp->classPath, scriptComp->className);
    } else {
        newInstance = cre_pkpy_instance_cache_add2(rootNode->entity);
    }
    py_assign(py_retval(), newInstance);
    return true;
}

bool cre_pkpy_api_packed_scene_load(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* path = py_tostr(py_arg(0));

    const CreSceneCacheId cacheId = cre_scene_template_cache_load_scene(path);
    py_newint(py_retval(), (py_i64)cacheId);
    return true;
}

// Collision Handler

bool cre_pkpy_api_collision_handler_process_collisions(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    const CollisionResult collisionResult = cre_collision_process_entity_collisions(entity);
    py_newlistn(py_retval(), (int)collisionResult.collidedEntityCount);
    for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
        const SkaEntity collidedEntity = collisionResult.collidedEntities[i];
        py_list_setitem(py_retval(), (int)i, cre_pkpy_instance_cache_add2(collidedEntity));
    }
    return true;
}

bool cre_pkpy_api_collision_handler_process_mouse_collisions(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_float); PY_CHECK_ARG_TYPE(1, tp_float); PY_CHECK_ARG_TYPE(2, tp_float); PY_CHECK_ARG_TYPE(3, tp_float);
    const f64 offsetX = py_tofloat(py_arg(0));
    const f64 offsetY = py_tofloat(py_arg(1));
    const f64 sizeW = py_tofloat(py_arg(2));
    const f64 sizeH = py_tofloat(py_arg(3));

    const SkaVector2 positionOffset = { .x = (f32)offsetX, .y = (f32)offsetY };
    const SkaVector2 mouseWorldPos = cre_pkpy_api_helper_mouse_get_global_position(&positionOffset);
    const SkaRect2 collisionRect = { mouseWorldPos.x, mouseWorldPos.y, (f32)sizeW, (f32)sizeH };
    const CollisionResult collisionResult = cre_collision_process_mouse_collisions(&collisionRect);
    py_newlistn(py_retval(), (int)collisionResult.collidedEntityCount);
    for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
        const SkaEntity collidedEntity = collisionResult.collidedEntities[i];
        py_list_setitem(py_retval(), (int)i, cre_pkpy_instance_cache_add2(collidedEntity));
    }
    return true;
}

// Network

bool cre_pkpy_api_network_is_server(int argc, py_StackRef argv) {
    py_newbool(py_retval(), ska_network_is_server());
    return true;
}

// Server


bool cre_pkpy_api_server_start(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 port = py_toint(py_arg(0));

    ska_udp_server_initialize((int32)port, ska_ecs_system_event_network_callback);
    return true;
}

bool cre_pkpy_api_server_stop(int argc, py_StackRef argv) {
    ska_udp_server_finalize();
    return true;
}

bool cre_pkpy_api_server_send(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* message = py_tostr(py_arg(0));

    ska_udp_server_send_message(message);
    return true;
}

// Client

bool cre_pkpy_api_client_start(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_int);
    const char* host = py_tostr(py_arg(0));
    const py_i64 port = py_toint(py_arg(1));

    ska_udp_client_initialize(host, (int32)port, ska_ecs_system_event_network_callback);
    return true;
}

bool cre_pkpy_api_client_stop(int argc, py_StackRef argv) {
    ska_udp_client_finalize();
    return true;
}

bool cre_pkpy_api_client_send(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* message = py_tostr(py_arg(0));

    ska_udp_client_send_message(message);
    return true;
}

// Node

static void set_node_component_from_type(SkaEntity entity, const char* classPath, const char* className, NodeBaseType baseType) {

    // Set components that should be set for a base node (that has invoked .new() from scripting)
    ska_ecs_component_manager_set_component(entity, NODE_COMPONENT_INDEX, node_component_create_ex(className, baseType));
    ska_ecs_component_manager_set_component(entity, SCRIPT_COMPONENT_INDEX, script_component_create_ex(classPath, className, CreScriptContextType_PYTHON));

    const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(baseType);

    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_NODE2D)) {
        ska_ecs_component_manager_set_component(entity, TRANSFORM2D_COMPONENT_INDEX, transform2d_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_SPRITE)) {
        ska_ecs_component_manager_set_component(entity, SPRITE_COMPONENT_INDEX, sprite_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_ANIMATED_SPRITE)) {
        ska_ecs_component_manager_set_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX, animated_sprite_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_TEXT_LABEL)) {
        ska_ecs_component_manager_set_component(entity, TEXT_LABEL_COMPONENT_INDEX, text_label_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_COLLIDER2D)) {
        ska_ecs_component_manager_set_component(entity, COLLIDER2D_COMPONENT_INDEX, collider2d_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_COLOR_RECT)) {
        ska_ecs_component_manager_set_component(entity, COLOR_RECT_COMPONENT_INDEX, color_rect_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_PARALLAX)) {
        ska_ecs_component_manager_set_component(entity, PARALLAX_COMPONENT_INDEX, parallax_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_PARTICLES2D)) {
        ska_ecs_component_manager_set_component(entity, PARTICLES2D_COMPONENT_INDEX, particles2d_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_TILEMAP)) {
        ska_ecs_component_manager_set_component(entity, TILEMAP_COMPONENT_INDEX, tilemap_component_create());
    }
}

bool cre_pkpy_api_node_new(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str); PY_CHECK_ARG_TYPE(1, tp_str); PY_CHECK_ARG_TYPE(2, tp_int);
    const char* classPath = py_tostr(py_arg(0));
    const char* className = py_tostr(py_arg(1));
    const py_i64 nodeTypeFlag = py_toint(py_arg(2));

    const SkaEntity entity = ska_ecs_entity_create();
    py_Ref newInstance = cre_pkpy_instance_cache_add(entity, classPath, className);
    SceneTreeNode* newNode = cre_scene_tree_create_tree_node(entity, NULL);
    cre_scene_manager_stage_child_node_to_be_added_later(newNode);
    set_node_component_from_type(entity, classPath, className, (NodeBaseType)nodeTypeFlag);
    py_assign(py_retval(), newInstance);
    return true;
}

bool cre_pkpy_api_node_get_name(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    py_newstr(py_retval(), nodeComponent->name);
    return true;
}

bool cre_pkpy_api_node_add_child(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_int);
    const py_i64 childEntityId = py_toint(py_arg(0));
    const py_i64 parentEntityId = py_toint(py_arg(1));

    cre_scene_manager_add_node_as_child((SkaEntity)childEntityId, (SkaEntity)parentEntityId);
    return true;
}

bool cre_pkpy_api_node_get_child(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_str);
    const py_i64 parentEntityId = py_toint(py_arg(0));
    const char* childName = py_tostr(py_arg(1));

    const SkaEntity childEntity = cre_scene_manager_get_entity_child_by_name((SkaEntity)parentEntityId, childName);
    if (childEntity != SKA_NULL_ENTITY) {
        py_assign(py_retval(), cre_pkpy_instance_cache_add2(childEntity));
    } else {
        ska_logger_warn("Unable to find child '%s' from entity '%u'", childName, parentEntityId);
        py_newnone(py_retval());
    }
    return true;
}

bool cre_pkpy_api_node_get_children(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    if (cre_scene_manager_has_entity_tree_node(entity)) {
        const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(entity);
        py_newlistn(py_retval(), (int)parentTreeNode->childCount);
        for (size_t i = 0; i < parentTreeNode->childCount; i++) {
            const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
            py_list_setitem(py_retval(), (int)i, cre_pkpy_instance_cache_add2(childTreeNode->entity));
        }
    } else {
        ska_logger_warn("Doesn't have tree node when trying to get children from %u", entity);
        py_newlist(py_retval());
    }

    return true;
}

bool cre_pkpy_api_node_get_parent(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 childEntityId = py_toint(py_arg(0));

    const SkaEntity childEntity = (SkaEntity)childEntityId;
    if (cre_scene_manager_has_entity_tree_node(childEntity)) {
        const SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(childEntity);
        if (treeNode->parent) {
            py_assign(py_retval(), cre_pkpy_instance_cache_add2(treeNode->parent->entity));
        } else {
            py_newnone(py_retval());
        }
    }
    return true;
}

bool cre_pkpy_api_node_queue_deletion(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    if (!nodeComponent->queuedForDeletion) {
        if (cre_scene_manager_has_entity_tree_node(entity)) {
            SceneTreeNode* node = cre_scene_manager_get_entity_tree_node(entity);
            cre_queue_destroy_tree_node_entity_all(node);
        } else {
            ska_logger_warn("Entity not found in tree, skipping queue deletion.");
        }
    } else {
        ska_logger_warn("Node '%s' already queued for deletion!", nodeComponent->name);
    }
    return true;
}

bool cre_pkpy_api_node_is_queued_for_deletion(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    py_newbool(py_retval(), nodeComponent->queuedForDeletion);
    return true;
}

bool cre_pkpy_api_node_set_time_dilation(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int); PY_CHECK_ARG_TYPE(1, tp_float);
    const py_i64 entityId = py_toint(py_arg(0));
    const f64 timeDilation = py_tofloat(py_arg(1));

    const SkaEntity entity = (SkaEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    nodeComponent->timeDilation.value = (f32)timeDilation;
    cre_scene_manager_invalidate_time_dilation_nodes_with_children(entity);
    return true;
}

bool cre_pkpy_api_node_get_time_dilation(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    const f64 timeDilation = (f64)nodeComponent->timeDilation.value;
    py_newfloat(py_retval(), timeDilation);
    return true;
}

bool cre_pkpy_api_node_get_total_time_dilation(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);
    const py_i64 entityId = py_toint(py_arg(0));

    const SkaEntity entity = (SkaEntity)entityId;
    const f64 totalTimeDilation = (f64)cre_scene_manager_get_node_full_time_dilation(entity);
    py_newfloat(py_retval(), totalTimeDilation);
    return true;
}

// Node2D

bool cre_pkpy_api_node2d_set_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_global_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_z_index(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_z_index(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_z_index_relative_to_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_z_index_relative_to_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_ignore_camera(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_ignore_camera(int argc, py_StackRef argv) { return true; }

// Sprite

bool cre_pkpy_api_sprite_get_texture(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_texture(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_draw_source(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_draw_source(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_shader_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_shader_instance(int argc, py_StackRef argv) { return true; }

// Animated Sprite

bool cre_pkpy_api_animated_sprite_play(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_current_animation_frame(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_add_animation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_stagger_animation_start_times(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_stagger_animation_start_times(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_shader_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_shader_instance(int argc, py_StackRef argv) { return true; }

// Text Label

bool cre_pkpy_api_text_label_get_text(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_text(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_font_uid(int argc, py_StackRef argv) { return true; }

// Collider2D

bool cre_pkpy_api_collider2d_get_extents(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_set_extents(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_set_color(int argc, py_StackRef argv) { return true; }

// Color Rect

bool cre_pkpy_api_color_rect_get_size(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_set_size(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_set_color(int argc, py_StackRef argv) { return true; }

// Parallax

bool cre_pkpy_api_parallax_get_scroll_speed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_parallax_set_scroll_speed(int argc, py_StackRef argv) { return true; }

// Particles2D

bool cre_pkpy_api_particles2d_get_amount(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_amount(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_life_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_life_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_damping(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_damping(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_explosiveness(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_explosiveness(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_initial_velocity(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_initial_velocity(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_spread(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_spread(int argc, py_StackRef argv) { return true; }
