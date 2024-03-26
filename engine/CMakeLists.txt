cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_C_STANDARD 11)

project(crescent_core C)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    list(APPEND flags "/W3" "/Zc:preprocessor")
elseif (APPLE)
    list(APPEND flags "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-bad-function-cast" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
else ()
    list(APPEND flags "-std=c11" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
endif ()

file(GLOB_RECURSE CRESCENT_SRC src/*.c src/*.h)
add_library(${PROJECT_NAME} STATIC ${CRESCENT_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED IS_CI_BUILD)
    set(IS_CI_BUILD "false")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC Ws2_32 seika cJSON pocketpy)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC -lmingw32 -static-libgcc -lws2_32 seika cJSON pocketpy)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC -Xlinker m seika cJSON pocketpy)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC -static-libgcc -Xlinker -export-dynamic m seika cJSON pocketpy)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})

# Copy directories over that are needed to run the engine and games
if (IS_CI_BUILD STREQUAL "false")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../assets
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../test_games
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../test_games
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../crescent_py_api
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../crescent_py_api
    )

    # Copy directories over that are needed to test
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../engine/test
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../engine/test
    )
endif()
