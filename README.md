# Crescent

[![windows-msvc-build Actions Status](https://github.com/Chukobyte/crescent/workflows/windows-msvc-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![ubuntu-gcc-build Actions Status](https://github.com/Chukobyte/crescent/workflows/ubuntu-gcc-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![windows-mingw-build Actions Status](https://github.com/Chukobyte/crescent/workflows/windows-mingw-build/badge.svg)](https://github.com/Chukobyte/crescent/actions)
[![pvs-studio Actions Status](https://github.com/Chukobyte/crescent/workflows/pvs-studio/badge.svg)](https://github.com/Chukobyte/crescent/actions)

<p align="center">
  <a href="https://chukobyte.github.io/crescent">
    <img src="assets/images/logo_2_enlarged.png" width="334" alt="Crescent Engine logo">
  </a>
</p>

A work in progress fighting game engine for Windows and Linux with the core written in C11.  The name is temporary and will very likely change in the near future.  Road map for the project coming soon.

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
    * [MiniAudio](https://github.com/mackron/miniaudio) - Audio
    * [Catch2](https://github.com/catchorg/Catch2) - Testing

## Project Configuration

Configuration is configured in the project's root directory with the `cre_config.py`.  It should look like the following:

```python
from crescent_api import *

configure_game(
  game_tile="Test Game",
  window_width=800,
  window_height=600,
  resolution_width=800,
  resolution_height=600,
  target_fps=66,
  initial_node_path="nodes/main_node.py",
  colliders_visible=False,
)

configure_assets(
  audio_sources=[
    AudioSource(
      file_path="assets/audio/sfx/rainbow_orb.wav"
    ),
  ],
  textures=[
    Texture(
      file_path="assets/images/characters/mor/mor_idle_sheet.png",
      wrap_s="clamp_to_border",
      wrap_t="clamp_to_border",
      filter_min="nearest",
      filter_mag="nearest",
    ),
  ],
  fonts=[
    Font(
      file_path="assets/fonts/verdana.ttf",
      uid="verdana-32",
      size=32,
    )
  ],
)

configure_inputs(
  input_actions=[
    InputAction(name="exit", values=[Input.Keyboard.ESC]),
    InputAction(name="play_sfx", values=[Input.Keyboard.SPACE]),
  ]
)
```

## Building

Supports Windows and linux builds at the moment. These environment variables are required to be set before building with make.

| Environment Variable  |                      Description                   | Required | Default Value |
|:---------------------:|:--------------------------------------------------:|:--------:|:-------------:|
|   SDL2_INCLUDE_PATH   | The include directory of SDL2.                     |    YES   |      N/A      |
|    SDL2_LIBS_PATH     | The libs directory of SDL2.                        |    YES   |      N/A      |
|  PYTHON_INCLUDE_PATH  | The include directory of Python.                   |    YES   |      N/A      |
|   PYTHON_LIBS_PATH    | The libs directory of Python.                      |    YES   |      N/A      |
| FREETYPE_INCLUDE_PATH | The include directory of Freetype.                 |    YES   |      N/A      |
|  FREETYPE_LIBS_PATH   | The libs directory of Freetype.                    |    YES   |      N/A      |

Crescent engine can be either built with make or cmake.  All build command should be executed from the project's root directory.

### Make

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

### CMake

**MinGW build**
1. Install MinGW through MSYS2
2. Install the following packages:
   * `pacman -S mingw-w64-x86_64-SDL2`
   * `pacman -S mingw-w64-x86_64-freetype`
   * `pacman -S mingw-w64-x86_64-python`
3. Run cmake
