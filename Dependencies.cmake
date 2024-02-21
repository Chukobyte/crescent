function(install_seika)
    if (NOT TARGET seika)
        # Setting to 10,000 as the default 200,000 is too much for our needs now
        set(SKA_MAX_ENTITIES 10000 CACHE STRING "Maximum number of entities")
        add_definitions(-DSKA_MAX_ENTITIES=${SKA_MAX_ENTITIES})

        include(FetchContent)
        FetchContent_Declare(
                seika
                GIT_REPOSITORY https://github.com/Chukobyte/seika.git
                GIT_TAG v0.0.37
        )
        FetchContent_MakeAvailable(seika)
    endif ()
endfunction()

function(install_pocketpy)
    if (NOT TARGET pocketpy)
        include(FetchContent)
        option(PK_BUILD_STATIC_LIB "Build static library" ON)
        FetchContent_Declare(
                pocketpy_content
                GIT_REPOSITORY https://github.com/blueloveTH/pocketpy.git
                GIT_TAG v1.4.1
        )
        FetchContent_MakeAvailable(pocketpy_content)
    endif ()
    target_include_directories(${PROJECT_NAME} PUBLIC
            "${PROJECT_BINARY_DIR}"
            "../include"
            "${pocketpy_content_SOURCE_DIR}/include"
    )
endfunction()
