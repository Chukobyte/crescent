cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)

project(roll_back_editor)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND flags "/W3" "/Zc:preprocessor")
else ()
    list(APPEND flags "-w" "-std=c++20" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings"
            "-Wno-deprecated-declarations" "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter"
            "-Wno-missing-field-initializers")
endif ()

find_package(SDL2 CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package (Python3 COMPONENTS Interpreter Development REQUIRED)

add_library(${PROJECT_NAME} STATIC
        # C++
        src/core/editor.cpp
        src/core/editor_context.cpp
        src/core/project_properties.cpp
        src/core/ui/opened_project_ui.cpp
        src/core/ui/project_manager_ui.cpp
        src/core/ui/imgui/imgui_handler.cpp
        src/core/ui/imgui/imgui_helper.cpp
        ../include/imgui.cpp
        ../include/imgui_demo.cpp
        ../include/imgui_draw.cpp
        ../include/imgui_tables.cpp
        ../include/imgui_widgets.cpp
        ../include/imgui/imgui_impl_opengl3.cpp
        ../include/imgui/imgui_impl_sdl.cpp
        # C
        ../engine/src/core/game_properties.c
        ../engine/src/core/engine_context.c
        ../engine/src/core/asset_manager.c
        ../engine/src/core/math/rbe_math.c
        ../engine/src/core/utils/logger.c
        ../engine/src/core/utils/rbe_file_system_utils.c
        ../engine/src/core/utils/rbe_string_util.c
        ../engine/src/core/data_structures/rbe_hash_map.c
        ../engine/src/core/data_structures/rbe_hash_map_string.c
        ../engine/src/core/memory/rbe_mem.c
        ../engine/src/core/scene/scene_manager.c
        ../engine/src/core/input/input.c
        ../engine/src/core/input/input_action.c
        ../engine/src/core/scripting/script_context.c
        ../engine/src/core/scripting/python/py_cache.c
        ../engine/src/core/scripting/python/py_helper.c
        ../engine/src/core/scripting/python/py_script_context.c
        ../engine/src/core/scripting/python/rbe_py.c
        ../engine/src/core/scripting/python/rbe_py_api_module.c
        ../engine/src/core/scripting/native/native_script_class.c
        ../engine/src/core/scripting/native/native_script_context.c
        ../engine/src/core/scripting/native/internal_classes/fps_display_class.c
        ../engine/src/core/ecs/ecs_manager.c
        ../engine/src/core/ecs/system/animated_sprite_rendering_ec_system.c
        ../engine/src/core/ecs/system/ec_system.c
        ../engine/src/core/ecs/system/font_rendering_ec_system.c
        ../engine/src/core/ecs/system/script_ec_system.c
        ../engine/src/core/ecs/system/sprite_rendering_ec_system.c
        ../engine/src/core/ecs/system/collision_ec_system.c
        ../engine/src/core/ecs/component/animated_sprite_component.c
        ../engine/src/core/ecs/component/collider2d_component.c
        ../engine/src/core/ecs/component/node_component.c
        ../engine/src/core/ecs/component/text_label_component.c
        ../engine/src/core/ecs/component/transform2d_component.c
        ../engine/src/core/ecs/component/script_component.c
        ../engine/src/core/ecs/component/sprite_component.c
        ../engine/src/core/ecs/component/component.c
        ../engine/src/core/networking/rbe_network.c
        ../engine/src/core/rendering/font.c
        ../engine/src/core/rendering/renderer.c
        ../engine/src/core/rendering/render_context.c
        ../engine/src/core/rendering/shader.c
        ../engine/src/core/rendering/texture.c
        ../engine/src/core/audio/audio.c
        ../engine/src/core/audio/audio_manager.c
        ../include/stb_image/stb_image.c
        ../include/glad/glad.c
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2 SDL2::SDL2main freetype Python3::Python Ws2_32)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC -lmingw32 -static-libgcc SDL2::SDL2 SDL2::SDL2main freetype Python3::Python -lws2_32)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})
