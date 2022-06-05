cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)

project(roll_back_editor)

# SDL2
cmake_path(SET SDL2_INCLUDE_PATH "$ENV{SDL2_INCLUDE}")
cmake_path(SET SDL2_LIBS_PATH "$ENV{SDL2_LIBS}")
# Python
cmake_path(SET PYTHON_INCLUDE_PATH "$ENV{PYTHON_INCLUDE}")
cmake_path(SET PYTHON_LIBS_PATH "$ENV{PYTHON_LIBS}")
# Freetype
cmake_path(SET FREETYPE_INCLUDE_PATH "$ENV{FREETYPE_INCLUDE}")
cmake_path(SET FREETYPE_LIBS_PATH "$ENV{FREETYPE_LIBS}")

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
        ../engine/src/core/utils/logger.c
        ../engine/src/core/utils/rbe_file_system_utils.c
        ../engine/src/core/data_structures/rbe_hash_map.c
        ../engine/src/core/data_structures/rbe_hash_map_string.c
        ../engine/src/core/memory/rbe_mem.c
        ../engine/src/core/scripting/python/rbe_py.c
        ../engine/src/core/scripting/python/py_helper.c
        ../engine/src/core/scripting/python/rbe_py_api_module.c
        ../include/glad/glad.c
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        "${SDL2_INCLUDE_PATH}"
        "${PYTHON_INCLUDE_PATH}"
        "${FREETYPE_INCLUDE_PATH}"
        )

target_link_directories(${PROJECT_NAME} PUBLIC
        ${SDL2_LIBS_PATH}
        ${PYTHON_LIBS_PATH}
        ${FREETYPE_LIBS_PATH}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC -lmingw32 -static-libgcc -lSDL2main -lSDL2 -lpython37 -lfreetype)
target_compile_options(${PROJECT_NAME} PUBLIC -w -std=c++20 -Wfatal-errors -Wall -Wextra -Wno-write-strings -Wno-deprecated-declarations -Wno-unused-variable -Wno-cast-function-type -Wno-unused-parameter -Wno-missing-field-initializers)
