from crescent_api import *

configure_game(
    game_title="Default Crescent Project",
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
        AudioSource(file_path="assets/audio/sfx/rainbow_orb.wav"),
    ],
    textures=[],
    fonts=[
        Font(
            file_path="assets/fonts/fight_font.ttf",
            uid="fight-32",
            size=32,
        ),
    ],
)

configure_inputs(
    input_actions=[
        InputAction(
            name="exit", values=[Input.Keyboard.ESC, Input.Gamepad.BUTTON_BACK]
        ),
    ]
)
