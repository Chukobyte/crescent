cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_C_STANDARD 11)

project(crescent_core C)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#    list(APPEND flags "/std:c11" "/W3" "/Zc:preprocessor") # TODO: Adding "/std:c11" breaks editor, fix later...
    list(APPEND flags "/W3" "/Zc:preprocessor")
else ()
    list(APPEND flags "-std=c11" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
endif ()

find_package(SDL2 CONFIG REQUIRED)

find_package(freetype CONFIG)
set(freetype_from_find_package TRUE)
if (NOT freetype_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(freetype REQUIRED freetype2)
    set(freetype_from_find_package FALSE)
endif()


find_package (Python3 COMPONENTS Interpreter Development REQUIRED)

add_library(${PROJECT_NAME} STATIC
        src/core/core.c
        src/core/game_properties.c
        src/core/engine_context.c
        src/core/scene/scene_manager.c
        src/core/scene/scene_utils.c
        src/core/scripting/script_context.c
        src/core/scripting/python/py_helper.c
        src/core/scripting/python/rbe_py.c
        src/core/scripting/python/rbe_py_api_module.c
        src/core/scripting/python/py_cache.c
        src/core/scripting/python/py_script_context.c
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
        src/core/ecs/component/text_label_component.c
        src/core/ecs/component/transform2d_component.c
        src/core/ecs/system/animated_sprite_rendering_ec_system.c
        src/core/ecs/system/color_rect_ec_system.c
        src/core/ecs/system/ec_system.c
        src/core/ecs/system/font_rendering_ec_system.c
        src/core/ecs/system/script_ec_system.c
        src/core/ecs/system/sprite_rendering_ec_system.c
        src/core/ecs/system/collision_ec_system.c
        src/core/json/json_file_loader.c
        )

if (NOT DEFINED IS_CI_BUILD)
    set(IS_CI_BUILD "false")
endif()

if (NOT TARGET glad)
    add_library(glad ../include/glad/glad.c)
endif()
if (NOT TARGET stb_image)
    add_library(stb_image ../include/stb_image/stb_image.c)
endif()
add_library(cJSON ../include/cjson/cJSON.c)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )
target_include_directories(glad PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )
target_include_directories(stb_image PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )


if (NOT freetype_from_find_package)
    target_include_directories(${PROJECT_NAME} PUBLIC ${freetype_INCLUDE_DIRS})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON SDL2::SDL2main SDL2::SDL2 freetype Python3::Python Ws2_32)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON -lmingw32 -static-libgcc SDL2::SDL2main SDL2::SDL2 freetype Python3::Python -lws2_32)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image cJSON -static-libgcc -Xlinker -export-dynamic SDL2::SDL2main SDL2::SDL2 freetype Python3::Python m)
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
endif()
