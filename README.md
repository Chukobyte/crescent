# Crescent

[![windows-msvc-build Actions Status](https://github.com/Chukobyte/crescent/workflows/windows-msvc-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![windows-mingw-build Actions Status](https://github.com/Chukobyte/crescent/workflows/windows-mingw-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![ubuntu-gcc-build Actions Status](https://github.com/Chukobyte/crescent/workflows/ubuntu-gcc-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![macosx-clang-build Actions Status](https://github.com/Chukobyte/crescent/workflows/macosx-clang-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![pvs-studio Actions Status](https://github.com/Chukobyte/crescent/workflows/pvs-studio/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![package-and-deploy Actions Status](https://github.com/Chukobyte/crescent/workflows/package-and-deploy/badge.svg)](https://github.com/Chukobyte/crescent/actions)

<p align="center">
  <a href="https://chukobyte.github.io/crescent">
    <img src="assets/images/logo_2_enlarged.png" width="334" alt="Crescent Engine logo">
  </a>
</p>

A work in progress fighting game engine for windows, linux, and macos with the core written in C11.  To get an idea of what's planned and what's actively in development checkout the [project page](https://github.com/users/Chukobyte/projects/1).  There is also documentation (not complete) which can be found [here](https://chukobyte.github.io/crescent/).

## Type of Games

Technically any game type will be able to be made with the engine.  But these are the primary types the engine will target:
- Traditional 2D fighting games such as Street Fighter.
- Beat em up games such as Streets of Rage.
- 3D Fighters such as Tekken.
- Platform fighters such as Super Smash Bros.

## Tech Stack

* Engine
  * [C](https://en.wikipedia.org/wiki/C_(programming_language)) (c11) - Engine Core
  * [SDL2](https://github.com/libsdl-org/SDL) - Windowing, inputs
  * [MiniAudio](https://github.com/mackron/miniaudio) - Audio
  * [Python](https://en.wikipedia.org/wiki/Python_(programming_language)) (3.10) - Scripting
  * [Unity](https://github.com/ThrowTheSwitch/Unity) - Testing
* Editor
    * [C++](https://en.wikipedia.org/wiki/C%2B%2B) (c++20) - Editor Core
    * [SDL2](https://github.com/libsdl-org/SDL) - Windowing, inputs
    * [ImGui](https://github.com/ocornut/imgui) - GUI Library
    * [ImPlot](https://github.com/epezent/implot) - Plotting Library (Used with ImGui)
    * [MiniAudio](https://github.com/mackron/miniaudio) - Audio
    * [Catch2](https://github.com/catchorg/Catch2) - Testing

## Building

Supports windows, linux, and macos builds at the moment.  Crescent engine can be either built with cmake or make.  All build commands should be executed from the project's root directory.

### CMake

**MinGW build**
1. Install MinGW through MSYS2
2. Update pacman: `pacman -Syu`
3. Install the following packages:
    * `pacman -S mingw-w64-x86_64-SDL2`
    * `pacman -S mingw-w64-x86_64-freetype`
    * `pacman -S mingw-w64-x86_64-python`
4. Run cmake

### Make

*Note: Currently outdated*

These environment variables are required to be set before building with make.

| Environment Variable  |                      Description                   | Required | Default Value |
|:---------------------:|:--------------------------------------------------:|:--------:|:-------------:|
|   SDL2_INCLUDE_PATH   | The include directory of SDL2.                     |    YES   |      N/A      |
|    SDL2_LIBS_PATH     | The libs directory of SDL2.                        |    YES   |      N/A      |
|  PYTHON_INCLUDE_PATH  | The include directory of Python.                   |    YES   |      N/A      |
|   PYTHON_LIBS_PATH    | The libs directory of Python.                      |    YES   |      N/A      |
| FREETYPE_INCLUDE_PATH | The include directory of Freetype.                 |    YES   |      N/A      |
|  FREETYPE_LIBS_PATH   | The libs directory of Freetype.                    |    YES   |      N/A      |


Engine:

```shell
# Build
make build-engine
# Run
make run-engine
# Clean
make clean-engine
```

Editor:

```shell
# Build
make build-editor
# Run
make run-editor
# Clean
make clean-editor
```
