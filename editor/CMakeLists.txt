cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)

project(crescent_editor)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND flags "/std:c++20" "/W3" "/Zc:preprocessor")
elseif (APPLE)
    list(APPEND flags "-w" "-std=c++2a" "-stdlib=libc++" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings"
            "-Wno-deprecated-declarations" "-Wno-unused-variable" "-Wno-bad-function-cast" "-Wno-unused-parameter"
            "-Wno-missing-field-initializers")
else ()
    list(APPEND flags "-w" "-std=c++20" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings"
            "-Wno-deprecated-declarations" "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter"
            "-Wno-missing-field-initializers")
endif ()

find_package(SDL2 CONFIG REQUIRED)

add_library(imgui STATIC
        ../include/imgui.cpp
        ../include/imgui_demo.cpp
        ../include/imgui_draw.cpp
        ../include/imgui_tables.cpp
        ../include/imgui_widgets.cpp
        ../include/imgui_impl_opengl3.cpp
        ../include/imgui_impl_sdl.cpp
        )

target_include_directories(imgui PRIVATE
        ${SDL2_INCLUDE_DIRS}
        )

add_library(implot STATIC
        ../include/implot.cpp
        ../include/implot_demo.cpp
        ../include/implot_items.cpp
        )

add_library(${PROJECT_NAME} STATIC
        src/core/asset_manager.cpp
        src/core/file_node.cpp
        src/core/editor.cpp
        src/core/editor_callbacks.cpp
        src/core/editor_context.cpp
        src/core/editor_settings.cpp
        src/core/project_properties.cpp
        src/core/asset_browser.cpp
        src/core/game_exporter.cpp
        src/core/file_creation/config_file_creator.cpp
        src/core/file_creation/scene_file_creator.cpp
        src/core/scene/scene_manager.cpp
        src/core/ui/opened_project_ui.cpp
        src/core/ui/project_manager_ui.cpp
        src/core/ui/imgui/imgui_file_browser.cpp
        src/core/ui/imgui/imgui_handler.cpp
        src/core/ui/imgui/imgui_helper.cpp
        src/core/ui/imgui/imgui_styler.cpp
        src/core/ui/imgui/imgui_window_renderer.cpp
        src/core/ui/views/opened_project/menu_bar_ui.cpp
        src/core/ui/views/opened_project/windows/animation_editor_ui.cpp
        src/core/ui/views/opened_project/windows/asset_browser_ui.cpp
        src/core/ui/views/opened_project/windows/asset_import_ui.cpp
        src/core/ui/views/opened_project/windows/console_ui.cpp
        src/core/ui/views/opened_project/windows/details_ui.cpp
        src/core/ui/views/opened_project/windows/scene_outliner_ui.cpp
        src/core/ui/views/opened_project/windows/scene_view_ui.cpp
        src/core/utils/console_logger.cpp
        src/core/utils/curve_editor.cpp
        src/core/utils/process_runner/process_runner.cpp
        src/core/utils/process_runner/process_context.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image SDL2::SDL2main SDL2::SDL2 imgui implot)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -lmingw32 -static-libgcc -fcoroutines SDL2::SDL2main SDL2::SDL2 imgui implot)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -static-libstdc++ -Xlinker SDL2::SDL2main SDL2::SDL2 imgui implot)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image -static-libgcc -Xlinker -export-dynamic SDL2::SDL2main SDL2::SDL2 imgui implot)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})

if (NOT DEFINED IS_CI_BUILD)
    set(IS_CI_BUILD "false")
endif()

if (WIN32)
    set(OS_BIN_DIR "windows")
elseif (APPLE)
    set(OS_BIN_DIR "macosx")
else ()
    set(OS_BIN_DIR "linux")
endif()

# Create bin and export directories to be used by the editor
if (IS_CI_BUILD STREQUAL "false")
    # Copy files for font awesome
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/../icons.yml
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../icons.yml
            )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/../fa-solid-900.ttf
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../fa-solid-900.ttf
            )

    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin
            )
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}
            )
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../export/crescent
            )
    # Copy files needed to run the engine within the editor
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../crescent_engine${CMAKE_EXECUTABLE_SUFFIX}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/crescent_engine${CMAKE_EXECUTABLE_SUFFIX}
            )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/assets
            )
    # For export
    # TODO: Fix this
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../crescent_engine_editor${CMAKE_EXECUTABLE_SUFFIX}
#            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../export/crescent/crescent${CMAKE_EXECUTABLE_SUFFIX}
#            )
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_directory
#            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin
#            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../export/crescent/bin
#            )
endif()

# Copy DLLs
if (IS_CI_BUILD STREQUAL "false")

    if (WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../brotlicommon.dll
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/brotlicommon.dll
                )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../brotlidec.dll
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/brotlidec.dll
                )
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bz2d.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/bz2d.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../freetyped.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/freetyped.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../libpng16d.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/libpng16d.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../python310_d.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/python310_d.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../SDL2d.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/SDL2d.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../zlibd1.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/zlibd1.dll
                    )
        else()
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bz2.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/bz2.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../freetype.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/freetype.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../libpng16.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/libpng16.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../python310.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/python310.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../SDL2.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/SDL2.dll
                    )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../zlib1.dll
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../bin/${OS_BIN_DIR}/zlib1.dll
                    )
        endif()
    endif()
endif()
