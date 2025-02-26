cmake_minimum_required(VERSION 3.27.0)

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

file(GLOB_RECURSE CRESCENT_EDITOR_SRC src/core/*.cpp src/core/*.h)
add_library(${PROJECT_NAME} STATIC ${CRESCENT_EDITOR_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC imgui implot seika font_awesome nlohmann_json zip)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC -lmingw32 -static-libgcc imgui implot seika font_awesome nlohmann_json zip)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC -Xlinker imgui implot seika font_awesome nlohmann_json zip)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC -static-libgcc -Xlinker -export-dynamic imgui implot seika font_awesome nlohmann_json zip)
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
if (NOT IS_CI_BUILD)
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
endif()
