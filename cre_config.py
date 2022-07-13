from crescent_api import *

configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=600,
    target_fps=66,
    initial_node_path="test_games/fighter_test/nodes/title_screen_node.py",
    colliders_visible=True,
)

configure_assets(
    audio_sources=[
        AudioSource(
            file_path="test_games/fighter_test/assets/audio/sfx/rainbow_orb.wav"
        ),
    ],
    textures=[
        Texture(
            file_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
            wrap_s="clamp_to_border",
            wrap_t="clamp_to_border",
            filter_min="nearest",
            filter_mag="nearest",
        ),
        Texture(
            file_path="test_games/fighter_test/assets/images/characters/mor/mor_still.png",
            wrap_s="clamp_to_border",
            wrap_t="clamp_to_border",
            filter_min="nearest",
            filter_mag="nearest",
        ),
    ],
    fonts=[
        Font(
            file_path="test_games/fighter_test/assets/fonts/verdana.ttf",
            uid="verdana-32",
            size=32,
        ),
        Font(
            file_path="test_games/fighter_test/assets/fonts/fight_font.ttf",
            uid="fight-64",
            size=64,
        ),
    ],
)

configure_inputs(
    input_actions=[
        InputAction(
            name="exit", values=[Input.Keyboard.ESC, Input.Gamepad.BUTTON_BACK]
        ),
        InputAction(name="play_sfx", values=[Input.Keyboard.SPACE]),
        InputAction(name="ui_confirm", values=[Input.Keyboard.RETURN]),
        InputAction(name="p1_move_left", values=[Input.Keyboard.A]),
        InputAction(name="p1_move_right", values=[Input.Keyboard.D]),
        InputAction(name="p1_light_punch", values=[Input.Keyboard.Q]),
        InputAction(name="p2_move_left", values=[Input.Keyboard.LEFT]),
        InputAction(name="p2_move_right", values=[Input.Keyboard.RIGHT]),
        InputAction(name="p2_light_punch", values=[Input.Keyboard.NUM_0]),
        InputAction(name="camera_left", values=[Input.Keyboard.J]),
        InputAction(name="camera_right", values=[Input.Keyboard.L]),
        InputAction(name="camera_up", values=[Input.Keyboard.I]),
        InputAction(name="camera_down", values=[Input.Keyboard.K]),
    ]
)
