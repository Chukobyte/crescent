# Configuration

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

## Node Configuration

A scene is built from a tree of nodes.  These nodes can be configured in node configuration files like the one below:

```python
from crescent_api import *

create_stage_nodes(
    stage_nodes=[
        StageNode(
            name="TitleScreen",
            type="Node2D",
            tags=None,
            external_node_source=None,
            components=[
                Transform2DComponent(
                    position=Vector2(0.0, 0.0),
                    scale=Vector2(1.0, 1.0),
                    rotation=0.0,
                    z_index=0,
                    z_index_relative_to_parent=True,
                    ignore_camera=False,
                ),
                ScriptComponent(
                    class_path="test_games.fighter_test.src.title_screen",
                    class_name="TitleScreen",
                ),
            ],
            children=[
                StageNode(
                    name="InstructionsText",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(130.0, 200.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Press Enter to Play!",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="ModeText",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(80.0, 300.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Mode:",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="ConnectText",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(60.0, 390.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Waiting for connection...",
                            color=Color(255, 255, 255, 0),
                        ),
                    ],
                    children=[],
                ),
            ],
        )
    ]
)

```
