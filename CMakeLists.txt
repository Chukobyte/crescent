cmake_minimum_required(VERSION 3.20.0)

project(crescent_engine)

add_subdirectory(seika)
add_subdirectory(engine)
add_subdirectory(editor)

# Create seika test exe
add_executable(seika_test seika/test/main.c)
target_link_libraries(seika_test seika unity)

# Create engine exe
add_executable(${PROJECT_NAME} engine/src/main.c)
target_link_libraries(${PROJECT_NAME} crescent_core seika)

# Create test engine exe
add_executable(${PROJECT_NAME}_test engine/test/main.c)
target_link_libraries(${PROJECT_NAME}_test crescent_core seika unity)

# Create editor exe
add_executable(${PROJECT_NAME}_editor editor/src/main.cpp)
target_link_libraries(${PROJECT_NAME}_editor crescent_editor crescent_core seika)

# Create test editor exe
add_executable(${PROJECT_NAME}_editor_test editor/test/main.cpp editor/test/unit/bezier_curve_test.cpp)
target_link_libraries(${PROJECT_NAME}_editor_test crescent_editor crescent_core seika)
