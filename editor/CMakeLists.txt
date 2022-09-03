cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)

project(crescent_editor)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND flags "/std:c++20" "/W3" "/Zc:preprocessor")
else ()
    list(APPEND flags "-w" "-std=c++20" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings"
            "-Wno-deprecated-declarations" "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter"
            "-Wno-missing-field-initializers")
endif ()

find_package(SDL2 CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
        # C++
        src/core/editor.cpp
        src/core/editor_background_tasks.cpp
        src/core/editor_context.cpp
        src/core/project_properties.cpp
        src/core/asset_browser.cpp
        src/core/file_creation/config_file_creator.cpp
        src/core/scene/scene_manager.cpp
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
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image SDL2::SDL2main SDL2::SDL2)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -lmingw32 -static-libgcc -fcoroutines SDL2::SDL2main SDL2::SDL2)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -static-libgcc -Xlinker -export-dynamic SDL2::SDL2main SDL2::SDL2)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})
