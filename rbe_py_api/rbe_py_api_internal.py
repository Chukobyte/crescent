def input_add_action(action_name: str, value: list) -> None:
    pass


def configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=800,
    target_fps=66,
) -> None:
    pass


def configure_assets(audio_sources=None, textures=None, fonts=None) -> None:
    if fonts is None:
        fonts = []
    if textures is None:
        textures = []
    if audio_sources is None:
        audio_sources = []


def configure_inputs(input_actions=None) -> None:
    if input_actions is None:
        input_actions = []


def create_stage_nodes(stage_nodes: list) -> None:
    pass
