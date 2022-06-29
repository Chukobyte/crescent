cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_C_STANDARD 11)

project(roll_back_core C)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    list(APPEND flags "/std:c11" "/W3" "/Zc:preprocessor")
else ()
    list(APPEND flags "-std=c11" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
endif ()

find_package(SDL2 CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package (Python3 COMPONENTS Interpreter Development REQUIRED)

add_library(${PROJECT_NAME} STATIC
        src/core/core.c
        src/core/game_properties.c
        src/core/engine_context.c
        src/core/asset_manager.c
        src/core/memory/rbe_mem.c
        src/core/math/rbe_math.c
        src/core/input/input.c
        src/core/input/input_action.c
        src/core/scene/scene_manager.c
        src/core/scripting/script_context.c
        src/core/scripting/python/py_helper.c
        src/core/scripting/python/rbe_py.c
        src/core/scripting/python/rbe_py_api_module.c
        src/core/scripting/python/py_cache.c
        src/core/scripting/python/py_script_context.c
        src/core/scripting/native/native_script_context.c
        src/core/scripting/native/native_script_class.c
        src/core/scripting/native/internal_classes/fps_display_class.c
        src/core/networking/rbe_network.c
        src/core/rendering/font.c
        src/core/rendering/renderer.c
        src/core/rendering/render_context.c
        src/core/rendering/shader.c
        src/core/rendering/texture.c
        src/core/audio/audio_manager.c
        src/core/audio/audio.c
        src/core/thread/rbe_pthread.c
        src/core/thread/rbe_thread_pool.c
        src/core/utils/command_line_args_util.c
        src/core/utils/logger.c
        src/core/utils/rbe_file_system_utils.c
        src/core/utils/rbe_string_util.c
        src/core/data_structures/rbe_hash_map.c
        src/core/data_structures/rbe_hash_map_string.c
        src/core/physics/collision/collision.c
        src/core/ecs/ecs_manager.c
        src/core/ecs/component/animated_sprite_component.c
        src/core/ecs/component/component.c
        src/core/ecs/component/node_component.c
        src/core/ecs/component/script_component.c
        src/core/ecs/component/sprite_component.c
        src/core/ecs/component/text_label_component.c
        src/core/ecs/component/transform2d_component.c
        src/core/ecs/system/animated_sprite_rendering_ec_system.c
        src/core/ecs/system/ec_system.c
        src/core/ecs/system/font_rendering_ec_system.c
        src/core/ecs/system/script_ec_system.c
        src/core/ecs/system/sprite_rendering_ec_system.c
        src/core/ecs/system/collision_ec_system.c
        src/core/ecs/component/collider2d_component.c
        src/core/networking/rbe_network_socket.c
        )


add_library(glad ../include/glad/glad.c)
add_library(stb_image ../include/stb_image/stb_image.c)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image SDL2::SDL2 SDL2::SDL2main freetype Python3::Python Ws2_32)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -lmingw32 -static-libgcc SDL2::SDL2 SDL2::SDL2main freetype Python3::Python -lws2_32)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -static-libgcc -Xlinker -export-dynamic SDL2::SDL2 SDL2::SDL2main freetype Python3::Python)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/../rbe_config.py
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../rbe_config.py
        )

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
        ${PROJECT_SOURCE_DIR}/../rbe_py_api
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../rbe_py_api
        )
