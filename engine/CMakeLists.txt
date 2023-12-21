cmake_minimum_required(VERSION 3.25.0)

set(CMAKE_C_STANDARD 11)

project(crescent_core C)

include(../Functions.cmake)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    #    list(APPEND flags "/std:c11" "/W3" "/Zc:preprocessor") # TODO: Adding "/std:c11" breaks editor, fix later...
    list(APPEND flags "/W3" "/Zc:preprocessor")
elseif (APPLE)
    list(APPEND flags "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-bad-function-cast" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
else ()
    list(APPEND flags "-std=c11" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
endif ()

add_library(${PROJECT_NAME} STATIC
        src/core/core.c
        src/core/game_properties.c
        src/core/engine_context.c
        src/core/node_event.c
        src/core/world.c
        src/core/scene/scene_template_cache.c
        src/core/scene/scene_manager.c
        src/core/scene/scene_utils.c
        src/core/scripting/script_context.c
        src/core/scripting/python/pocketpy/cre_pkpy_util.c
        src/core/scripting/python/pocketpy/cre_pkpy_script_context.c
        src/core/scripting/python/pocketpy/cre_pkpy_entity_instance_cache.c
        src/core/scripting/python/pocketpy/cre_pkpy_node_event_manager.c
        src/core/scripting/python/pocketpy/api/cre_pkpy_api.c
        src/core/scripting/python/pocketpy/api/cre_pkpy_api_node.c
        src/core/scripting/native/native_script_context.c
        src/core/scripting/native/native_script_class.c
        src/core/scripting/native/internal_classes/fps_display_class.c
        src/core/utils/command_line_args_util.c
        src/core/physics/collision/collision.c
        src/core/camera/camera.c
        src/core/camera/camera_manager.c
        src/core/ecs/ecs_manager.c
        src/core/ecs/component/animated_sprite_component.c
        src/core/ecs/component/collider2d_component.c
        src/core/ecs/component/color_rect_component.c
        src/core/ecs/component/component.c
        src/core/ecs/component/node_component.c
        src/core/ecs/component/script_component.c
        src/core/ecs/component/sprite_component.c
        src/core/ecs/component/parallax_component.c
        src/core/ecs/component/particles2d_component.c
        src/core/ecs/component/text_label_component.c
        src/core/ecs/component/transform2d_component.c
        src/core/ecs/system/animated_sprite_rendering_ec_system.c
        src/core/ecs/system/color_rect_ec_system.c
        src/core/ecs/system/collision_ec_system.c
        src/core/ecs/system/ec_system.c
        src/core/ecs/system/font_rendering_ec_system.c
        src/core/ecs/system/parallax_ec_system.c
        src/core/ecs/system/particle_emitter_ec_system.c
        src/core/ecs/system/script_ec_system.c
        src/core/ecs/system/sprite_rendering_ec_system.c
        src/core/json/json_file_loader.c
        src/core/json/json_file_loader_scene.c
        src/core/json/json_helper.c
        src/core/math/curve_float_manager.c
)

if (NOT DEFINED IS_CI_BUILD)
    set(IS_CI_BUILD "false")
endif()

#--- Dependencies ---#

find_package(SDL2 CONFIG REQUIRED)

find_package(freetype CONFIG)
set(freetype_from_find_package TRUE)
if (NOT freetype_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(freetype REQUIRED freetype2)
    set(freetype_from_find_package FALSE)
endif()


find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Include pocketpy as a dependency
include(FetchContent)
option(PK_BUILD_STATIC_LIB "Build static library" ON)
FetchContent_Declare(
        pocketpy_content
        GIT_REPOSITORY https://github.com/blueloveTH/pocketpy.git
        GIT_TAG v1.3.2
)
FetchContent_MakeAvailable(pocketpy_content)

if (NOT TARGET glad)
    add_library(glad ../include/glad/glad.c)
endif()
if (NOT TARGET stb_image)
    add_library(stb_image ../include/stb_image/stb_image.c)
endif()
if (NOT TARGET unity)
    add_library(unity ../include/unity.c)
endif()

install_seika()

add_library(cJSON ../include/cjson/cJSON.c)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        "${pocketpy_content_SOURCE_DIR}/include"
)
target_include_directories(glad PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
)
target_include_directories(stb_image PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
)

target_include_directories(unity PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
)

if (NOT freetype_from_find_package)
    target_include_directories(${PROJECT_NAME} PUBLIC ${freetype_INCLUDE_DIRS})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON SDL2::SDL2main SDL2::SDL2 freetype Ws2_32 seika pocketpy)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON -lmingw32 -static-libgcc SDL2::SDL2main SDL2::SDL2 freetype -lws2_32 seika pocketpy)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON -Xlinker SDL2::SDL2main SDL2::SDL2 freetype m seika pocketpy)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON -static-libgcc -Xlinker -export-dynamic SDL2::SDL2main SDL2::SDL2 freetype m seika pocketpy)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})

# Copy directories over that are needed to run the engine and games
if (IS_CI_BUILD STREQUAL "false")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../assets
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../test_games
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../test_games
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../crescent_py_api
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../crescent_py_api
    )

    # Copy directories over that are needed to test
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../engine/test
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../engine/test
    )
endif()
