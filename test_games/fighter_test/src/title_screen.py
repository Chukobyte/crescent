from rbe_py_api import *
from test_games.fighter_test.src.game_state import *

GAME_MODES = [
    GameMode.LOCAL_PVP,
    GameMode.LOCAL_AI,
    GameMode.ONLINE_PVP_HOST,
    GameMode.ONLINE_PVP_CLIENT,
]


def get_game_mode(index: int) -> str:
    return GAME_MODES[index % len(GAME_MODES)]


class TitleScreen(Node2D):
    def _start(self) -> None:
        self.is_waiting_to_connect = False
        self.selected_game_mode = GameMode.LOCAL_PVP
        self.selected_game_mode_index = 0
        self.mode_text = self.get_child(name="ModeText")
        self._update_game_mode_text()

    def _update_game_mode_text(self) -> None:
        self.selected_game_mode = get_game_mode(self.selected_game_mode_index)
        self.mode_text.text = f"Mode: {self.selected_game_mode}"

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

        if self.is_waiting_to_connect:
            return None

        if Input.is_action_just_pressed(name="ui_confirm"):
            game_state = GameState()
            game_state.mode = self.selected_game_mode
            if game_state.mode == GameMode.ONLINE_PVP_HOST:
                Server.start(port=8888)
                Server.subscribe(
                    signal_id="client_connected",
                    listener_node=self,
                    listener_func=self._network_server_client_connected_callback,
                )
                self.is_waiting_to_connect = True
            elif game_state.mode == GameMode.ONLINE_PVP_CLIENT:
                Client.start("127.0.0.1", 8888)
                Client.subscribe(
                    signal_id="client_connected",
                    listener_node=self,
                    listener_func=self._network_server_client_connected_callback,
                )
                self.is_waiting_to_connect = True
            else:
                SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")

        if Input.is_action_just_pressed(name="p2_move_left"):
            self.selected_game_mode_index -= 1
            self._update_game_mode_text()
        elif Input.is_action_just_pressed(name="p2_move_right"):
            self.selected_game_mode_index += 1
            self._update_game_mode_text()

    def _network_server_client_connected_callback(self) -> None:
        SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")
