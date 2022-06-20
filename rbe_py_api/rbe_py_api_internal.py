def engine_exit(code=0) -> None:
    pass


def engine_set_target_fps(fps: float) -> None:
    pass


def engine_get_target_fps() -> int:
    return 66


def engine_get_average_fps() -> float:
    return 66.0


def engine_set_fps_display_enabled(enabled: bool) -> None:
    pass


def configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=800,
    target_fps=66,
    initial_node_path="",
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


def input_add_action(name: str, value: str) -> None:
    pass


def input_is_action_pressed(name: str) -> bool:
    return False


def input_is_action_just_pressed(name: str) -> bool:
    return False


def input_is_action_just_released(name: str) -> bool:
    return False


def scene_tree_change_scene(path: str) -> None:
    pass


def audio_manager_play_sound(path: str, loops: bool) -> None:
    pass


def audio_manager_stop_sound(path: str) -> None:
    pass


def node_get_child(entity_id: int, child_name: str) -> int:
    return 1


def node2D_set_position(entity_id: int, x: float, y: float) -> None:
    pass


def node2D_add_to_position(entity_id: int, x: float, y: float) -> None:
    pass


def node2D_get_position(entity_id: int) -> tuple:
    return 0.0, 0.0


def network_is_server() -> bool:
    return True


def server_start(port: int) -> None:
    pass


def server_stop() -> None:
    pass


def server_send(message: str) -> None:
    pass


def server_subscribe(signal_id: str, listener_node: int, listener_func) -> None:
    pass


def client_start(host: str, port: int) -> None:
    pass


def client_stop() -> None:
    pass


def client_send(message: str) -> None:
    pass


def client_subscribe(signal_id: str, listener_node: int, listener_func) -> None:
    pass
