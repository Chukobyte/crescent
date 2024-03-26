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

A work in progress fighting and beat em up game engine for windows, linux, and macos with the core written in C11.  To get an idea of what's planned and what's actively in development checkout the [project page](https://github.com/users/Chukobyte/projects/1).  There is also documentation (not complete) which can be found [here](https://chukobyte.github.io/crescent/).

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
  * [pocketpy](https://github.com/blueloveth/pocketpy) - Python Scripting
  * [Unity](https://github.com/ThrowTheSwitch/Unity) - Testing
  * [PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C code.
* Editor
  * [C++](https://en.wikipedia.org/wiki/C%2B%2B) (c++20) - Editor Core
  * [SDL2](https://github.com/libsdl-org/SDL) - Windowing, inputs
  * [ImGui](https://github.com/ocornut/imgui) - GUI Library
  * [ImPlot](https://github.com/epezent/implot) - Plotting Library (Used with ImGui)
  * [MiniAudio](https://github.com/mackron/miniaudio) - Audio
  * [Catch2](https://github.com/catchorg/Catch2) - Testing
  * [PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C++ code.
* All
  * [Seika](https://github.com/Chukobyte/seika) - Game framework that was originally part of crescent.

## Building

Supports windows, linux, and macos builds at the moment.  Crescent engine uses cmake to build.

### CMake

Crescent officially supports and is actively tested with the following compilers.

Windows:
- msvc
- mingw

Linux (Ubuntu):
- gcc

MacOS:
- clang

Example of running on command line in the project source directory:
```sh
cmake .
cmake --build .
```
