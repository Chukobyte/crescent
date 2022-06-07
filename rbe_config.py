from rbe_py_api import *

configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=600,
    target_fps=66,
)

create_assets(
    audio_sources=[
        AudioSource(
            file_path="test_games/fighter_test/assets/audio/sfx/rainbow_orb.wav"
        ),
    ],
    textures=[
        Texture(
            file_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
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
        )
    ],
)

configure_inputs(
    input_actions=[
        InputAction(name="exit", values=["esc"]),
        InputAction(name="play_sfx", values=["space"]),
    ]
)
