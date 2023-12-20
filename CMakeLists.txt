cmake_minimum_required(VERSION 3.25.0)

project(crescent_engine)

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
find_package(SDL2 CONFIG REQUIRED)
add_executable(${PROJECT_NAME}_editor_test editor/test/main.cpp editor/test/unit/curve_float_test.cpp editor/test/unit/game_exporter_test.cpp)
target_link_libraries(${PROJECT_NAME}_editor_test crescent_editor crescent_core SDL2::SDL2main SDL2::SDL2)
