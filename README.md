## Roll Back Engine

A work in progress fighting game engine with the core written in C11.  The name is temporary and will very likely change in the near future.

### Tech Stack

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

### Project Configuration

Configuration is configured in the project's root directory with the `rbe_config.py`.  It should look like the following:

```python
from rbe_py_api import *

configure_game(
  game_tile="Test Game",
  window_width=800,
  window_height=600,
  resolution_width=800,
  resolution_height=600,
  target_fps=66,
  initial_node_path="nodes/main_node.py",
)

configure_assets(
  audio_sources=[
    AudioSource(
      file_path="assets/audio/sfx/rainbow_orb.wav"
    ),
  ],
  textures=[
    Texture(
      file_path="assets/images/melissa_walk_animation.png",
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

### Building

Only supports MinGW and Windows at the moment. These environment variables are required to be set before building.

| Environment Variable  |                      Description                   | Required | Default Value |
|:---------------------:|:--------------------------------------------------:|:--------:|:-------------:|
|   SDL2_INCLUDE_PATH   | The include directory of SDL2.                     |    YES   |      N/A      |
|    SDL2_LIBS_PATH     | The libs directory of SDL2.                        |    YES   |      N/A      |
|  PYTHON_INCLUDE_PATH  | The include directory of Python.                   |    YES   |      N/A      |
|   PYTHON_LIBS_PATH    | The libs directory of Python.                      |    YES   |      N/A      |
| FREETYPE_INCLUDE_PATH | The include directory of Freetype.                 |    YES   |      N/A      |
|  FREETYPE_LIBS_PATH   | The libs directory of Freetype.                    |    YES   |      N/A      |

Roll back engine can be either built with make or cmake.  All build command should be executed from the project's root directory.

#### Make

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

#### CMake

*Note: CMake build currently doesn't work temporarily*

Run cmake command from top level directory

```shell
cmake -G "MinGW Makefiles" -B cmake-build-dev -S .
cmake --build cmake-build-dev
```
