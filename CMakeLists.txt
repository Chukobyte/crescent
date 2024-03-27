cmake_minimum_required(VERSION 3.27.0)

project(crescent_engine)

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
