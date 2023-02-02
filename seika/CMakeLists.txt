cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_C_STANDARD 11)

project(seika C)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#    list(APPEND flags "/std:c11" "/W3" "/Zc:preprocessor") # TODO: Adding "/std:c11" breaks editor, fix later...
    list(APPEND flags "/W3" "/Zc:preprocessor")
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
        src/seika.c
        src/asset/asset_manager.c
        src/asset/asset_file_loader.c
        src/memory/se_mem.c
        src/math/se_curve_float.c
        src/math/se_math.c
        src/input/input.c
        src/input/input_action.c
        src/input/mouse.c
        src/networking/se_network.c
        src/networking/se_network_socket.c
        src/rendering/font.c
        src/rendering/frame_buffer.c
        src/rendering/renderer.c
        src/rendering/render_context.c
        src/rendering/shader.c
        src/rendering/shader_instance.c
        src/rendering/texture.c
        src/audio/audio_manager.c
        src/audio/audio.c
        src/thread/se_pthread.c
        src/thread/se_thread_pool.c
        src/utils/logger.c
        src/utils/se_file_system_utils.c
        src/utils/se_string_util.c
        src/utils/observer.c
        src/data_structures/se_array_utils.c
        src/data_structures/se_queue.c
        src/data_structures/se_hash_map.c
        src/data_structures/se_hash_map_string.c
        src/data_structures/se_spatial_hash_map.c
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
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glad stb_image kuba_zip -lmingw32 -static-libgcc SDL2::SDL2main SDL2::SDL2 freetype -lws2_32)
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
