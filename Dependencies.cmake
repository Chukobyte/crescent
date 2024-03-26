if (NOT TARGET seika)
    # Setting to 10,000 as the default 200,000 is too much for our needs now
    set(SKA_MAX_ENTITIES 10000 CACHE STRING "Maximum number of entities")
    add_definitions(-DSKA_MAX_ENTITIES=${SKA_MAX_ENTITIES})

    include(FetchContent)
    FetchContent_Declare(
            seika_content
            GIT_REPOSITORY https://github.com/Chukobyte/seika.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(seika_content)
endif ()

if (NOT TARGET pocketpy)
    include(FetchContent)
    option(PK_BUILD_STATIC_LIB "Build static library" ON)
    FetchContent_Declare(
            pocketpy_content
            GIT_REPOSITORY https://github.com/blueloveTH/pocketpy.git
            GIT_TAG v1.4.2
    )
    FetchContent_MakeAvailable(pocketpy_content)
endif ()

if (NOT TARGET cJSON)
    add_library(cJSON "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cjson/cJSON.c")
    target_include_directories(cJSON PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
endif ()

