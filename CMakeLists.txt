cmake_minimum_required(VERSION 3.20.0)

project(roll_back_engine)

add_subdirectory(engine)
add_subdirectory(editor)

# Create engine exe
add_executable(${PROJECT_NAME} engine/src/main.c)
target_link_libraries(${PROJECT_NAME} roll_back_core)

# Create editor exe
add_executable(roll_back_engine_editor editor/src/main.cpp)
target_link_libraries(roll_back_engine_editor roll_back_editor roll_back_core)

# Validations
#if(NOT EXISTS "${PROJECT_BINARY_DIR}/SDL2.dll")
#    message(FATAL_ERROR "SDL2.dll is missing!")
#endif()
#if(NOT EXISTS "${PROJECT_BINARY_DIR}/SDL2_mixer.dll")
#    message(FATAL_ERROR "SDL2_mixer.dll is missing!")
#endif()
#if(NOT EXISTS "${PROJECT_BINARY_DIR}/python37.dll")
#    message(FATAL_ERROR "python37.dll is missing!")
#endif()
#if(NOT EXISTS "${PROJECT_BINARY_DIR}/libfreetype6.dll")
#    message(FATAL_ERROR "libfreetype-6.dll is missing!")
#endif()
