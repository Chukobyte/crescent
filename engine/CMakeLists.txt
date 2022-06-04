cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_C_STANDARD 99)

project(roll_back_core C)

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
        src/core/core.c
        src/core/game_properties.c
        src/core/memory/rbe_mem.c
        src/core/input/input.c
        src/core/input/input_action.c
        src/core/scripting/python/py_helper.c
        src/core/scripting/python/rbe_py.c
        src/core/scripting/python/rbe_py_api_module.c
        src/core/utils/command_line_args_util.c
        src/core/utils/logger.c
        src/core/utils/rbe_file_system_utils.c
        src/core/data_structures/rbe_hash_map.c
        src/core/data_structures/rbe_hash_map_string.c
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
target_compile_options(${PROJECT_NAME} PUBLIC -std=c99 -Wfatal-errors -Wall -Wextra -Wno-write-strings -Wno-deprecated-declarations -Wno-unused-variable -Wno-cast-function-type -Wno-unused-parameter -Wno-missing-field-initializers)
