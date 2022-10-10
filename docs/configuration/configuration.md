# Configuration

## Project Configuration

Configuration is configured in the project's root directory with the `project.ccfg`.  It should look like the following:

```json
{
    "name": "Default Crescent Project",
    "window_width": 800,
    "window_height": 600,
    "resolution_width": 800,
    "resolution_height": 600,
    "target_fps": 66,
    "initial_node_path": "nodes/main.cscn",
    "colliders_visible": false,
    "assets": {
        "audio_sources": [
            {
                "file_path": "assets/audio/sfx/rainbow_orb.wav"
            }
        ],
        "textures": [],
        "fonts": [
            {
                "file_path": "assets/fonts/fight_font.ttf",
                "uid": "fight-32",
                "size": 32
            }
        ]
    },
    "inputs": [
        {
            "name": "exit",
            "values": [
                "esc",
                "joystick_back"
            ]
        }
    ]
}
```

## Node Configuration

A scene is built from a tree of nodes.  These nodes can be configured in node configuration files (*.cscn) like the one below:

```json
{
  "name": "Main",
  "type": "Node2D",
  "tags": null,
  "external_node_source": null,
  "components": [
    {
      "type": "transform_2d",
      "position": {
        "x": 0.0,
        "y": 0.0
      },
      "scale": {
        "x": 1.0,
        "y": 1.0
      },
      "rotation": 0.0,
      "z_index": 0,
      "z_index_relative_to_parent": true,
      "ignore_camera": false
    },
    {
      "type": "script",
      "class_path": "src.main",
      "class_name": "Main"
    }
  ],
  "children": [
    {
      "name": "TestTextLabel",
      "type": "TextLabel",
      "tags": null,
      "external_node_source": null,
      "components": [
        {
          "type": "transform_2d",
          "position": {
            "x": 120.0,
            "y": 200.0
          },
          "scale": {
            "x": 1.0,
            "y": 1.0
          },
          "rotation": 0.0,
          "z_index": 0,
          "z_index_relative_to_parent": true,
          "ignore_camera": false
        },
        {
          "type": "text_label",
          "uid": "fight-32",
          "text": "Default Project, Press 'ESC' to exit!",
          "color": {
            "r": 255,
            "g": 255,
            "b": 255,
            "a": 255
          }
        }
      ],
      "children": []
    }
  ]
}
```
