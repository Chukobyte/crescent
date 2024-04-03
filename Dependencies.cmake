if (NOT TARGET seika)
    set(SDL_STATIC_VCRT ON)

    # Setting to 10,000 as the default 200,000 is too much for our needs now
    set(SKA_MAX_ENTITIES 10000 CACHE STRING "Maximum number of entities")
    add_definitions(-DSKA_MAX_ENTITIES=${SKA_MAX_ENTITIES})

    include(FetchContent)
    FetchContent_Declare(
            seika_content
            GIT_REPOSITORY https://github.com/Chukobyte/seika.git
            GIT_TAG v0.1.0
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

# https://github.com/DaveGamble/cJSON
if (NOT TARGET cJSON)
    add_library(cJSON "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cjson/cJSON.c")
    target_include_directories(cJSON PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
endif ()

if (NOT TARGET imgui)
    include(FetchContent)
    FetchContent_Declare(
            imgui_content
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG v1.90.4-docking
    )
    FetchContent_MakeAvailable(imgui_content)
    add_library(imgui STATIC
            ${imgui_content_SOURCE_DIR}/imgui.cpp
            ${imgui_content_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_content_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_content_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_content_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_content_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_content_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp)
    set_target_properties(imgui PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS OFF
    )
    target_include_directories(imgui PUBLIC ${imgui_content_SOURCE_DIR} ${imgui_content_SOURCE_DIR}/backends)
    target_link_libraries(imgui PRIVATE seika)
endif ()

if (NOT TARGET implot)
    include(FetchContent)
    FetchContent_Declare(
            implot_content
            GIT_REPOSITORY https://github.com/epezent/implot.git
            GIT_TAG v0.16
    )
    FetchContent_MakeAvailable(implot_content)

    add_library(implot STATIC
            ${implot_content_SOURCE_DIR}/implot.cpp
            ${implot_content_SOURCE_DIR}/implot_demo.cpp
            ${implot_content_SOURCE_DIR}/implot_items.cpp
    )
    set_target_properties(implot PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS OFF
    )
    target_include_directories(implot PUBLIC ${implot_content_SOURCE_DIR})
    target_link_libraries(implot PRIVATE imgui)
endif ()

# https://github.com/FortAwesome/Font-Awesome
if (NOT TARGET font_awesome)
    add_library(font_awesome INTERFACE thirdparty/font_awesome/IconsFontAwesome6.h)
    target_include_directories(font_awesome INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/font_awesome")
endif ()

if (NOT TARGET nlohmann_json)
    add_library(nlohmann_json INTERFACE thirdparty/nlohmann/json.hpp)
    target_include_directories(nlohmann_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
endif ()

if (NOT TARGET catch)
    add_library(catch INTERFACE thirdparty/catch/catch.hpp)
    target_include_directories(catch INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch")
endif ()
