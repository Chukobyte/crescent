cmake_minimum_required(VERSION 3.27.0)

project(crescent_engine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if ( MSVC )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
endif ()

option(IS_CI_BUILD "Should be set 'ON' for ci builds" OFF)

include(Dependencies.cmake)

add_subdirectory(engine)
add_subdirectory(editor)

# Create engine exe
add_executable(${PROJECT_NAME} engine/src/main.c)
target_link_libraries(${PROJECT_NAME} crescent_core)

# Create test engine exe
add_executable(${PROJECT_NAME}_test engine/test/main.c)
target_link_libraries(${PROJECT_NAME}_test crescent_core unity)

# Create editor exe
add_executable(${PROJECT_NAME}_editor editor/src/main.cpp)
add_dependencies(${PROJECT_NAME}_editor ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_editor crescent_editor crescent_core)

# Create test editor exe
add_executable(${PROJECT_NAME}_editor_test editor/test/main.cpp editor/test/unit/curve_float_test.cpp editor/test/unit/game_exporter_test.cpp)
target_link_libraries(${PROJECT_NAME}_editor_test crescent_editor crescent_core catch)
