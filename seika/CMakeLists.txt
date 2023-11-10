cmake_minimum_required(VERSION 3.25.0)

set(CMAKE_C_STANDARD 11)

project(seika C)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#    list(APPEND flags "/std:c11" "/W3" "/Zc:preprocessor") # TODO: Adding "/std:c11" breaks editor, fix later...
    list(APPEND flags "/W3" "/Zc:preprocessor")
elseif (APPLE)
    list(APPEND flags "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-bad-function-cast" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
else ()
    list(APPEND flags "-std=c11" "-Wfatal-errors" "-Wall" "-Wextra" "-Wno-write-strings" "-Wno-deprecated-declarations"
            "-Wno-unused-variable" "-Wno-cast-function-type" "-Wno-unused-parameter" "-Wno-missing-field-initializers")
endif ()

find_package(SDL2 CONFIG REQUIRED)

find_package(freetype CONFIG)
set(freetype_from_find_package TRUE)
if (NOT freetype_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(freetype REQUIRED freetype2)
    set(freetype_from_find_package FALSE)
endif()

add_library(${PROJECT_NAME} STATIC
        seika.c
        asset/asset_manager.c
        asset/asset_file_loader.c
        memory/se_mem.c
        math/se_curve_float.c
        math/se_math.c
        input/input.c
        input/input_action.c
        input/mouse.c
        networking/se_network.c
        networking/se_network_socket.c
        rendering/font.c
        rendering/frame_buffer.c
        rendering/renderer.c
        rendering/render_context.c
        rendering/shader/shader.c
        rendering/shader/shader_cache.c
        rendering/shader/shader_file_parser.c
        rendering/shader/shader_instance.c
        rendering/texture.c
        audio/audio_manager.c
        audio/audio.c
        thread/se_pthread.c
        thread/se_thread_pool.c
        utils/logger.c
        utils/se_file_system_utils.c
        utils/se_platform.c
        utils/se_string_util.c
        utils/observer.c
        data_structures/se_array_utils.c
        data_structures/se_array_list.c
        data_structures/se_queue.c
        data_structures/se_hash_map.c
        data_structures/se_hash_map_string.c
        data_structures/se_spatial_hash_map.c
        )

add_library(glad ../include/glad/glad.c)
add_library(stb_image ../include/stb_image/stb_image.c)
add_library(kuba_zip ../include/kuba_zip/zip.c)
add_library(unity ../include/unity.c)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )
target_include_directories(glad PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )
target_include_directories(stb_image PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )

target_include_directories(unity PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../include"
        )

if (NOT freetype_from_find_package)
    target_include_directories(${PROJECT_NAME} PUBLIC ${freetype_INCLUDE_DIRS})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image kuba_zip SDL2::SDL2main SDL2::SDL2 freetype Ws2_32)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image kuba_zip -lmingw32 -static-libgcc SDL2::SDL2main SDL2::SDL2 freetype -lws2_32)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image kuba_zip -Xlinker SDL2::SDL2main SDL2::SDL2 freetype m)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image kuba_zip -static-libgcc -Xlinker -export-dynamic SDL2::SDL2main SDL2::SDL2 freetype m)
endif ()

target_compile_options(${PROJECT_NAME} PUBLIC ${flags})

# Copy directories over that are needed to test
if (NOT DEFINED IS_CI_BUILD)
    set(IS_CI_BUILD "false")
endif()
if (IS_CI_BUILD STREQUAL "false")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/../seika/test
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../seika/test
            )
endif()
