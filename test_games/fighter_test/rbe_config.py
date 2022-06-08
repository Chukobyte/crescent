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
