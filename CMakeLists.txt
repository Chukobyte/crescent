cmake_minimum_required(VERSION 3.20.0)

project(crescent_engine)

add_subdirectory(seika)
add_subdirectory(engine)
add_subdirectory(editor)

# Create seika test exe
add_executable(seika_test seika/src/main.c)
target_link_libraries(seika_test seika)

# Create engine exe
add_executable(${PROJECT_NAME} engine/src/main.c)
target_link_libraries(${PROJECT_NAME} crescent_core seika)

# Create editor exe
add_executable(crescent_engine_editor editor/src/main.cpp)
target_link_libraries(crescent_engine_editor crescent_editor crescent_core seika)
