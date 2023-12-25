function(install_seika)
    if (NOT TARGET seika)
        include(FetchContent)
        FetchContent_Declare(
                seika
                GIT_REPOSITORY https://github.com/Chukobyte/seika.git
                GIT_TAG v0.0.14
        )
        FetchContent_MakeAvailable(seika)
    endif ()
endfunction()
