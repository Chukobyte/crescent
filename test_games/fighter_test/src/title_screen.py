from rbe_py_api import *
from test_games.fighter_test.src.game_state import *


class TitleScreen(Node2D):
    def _start(self) -> None:
        self.selected_game_mode = GameMode.LOCAL_PVP
        # self.mode_text = self.get_child(name="ModeText")
        # print(self.mode_text)
        # self.mode_text.text = f"Mode: {self.selected_game_mode}"

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

        if Input.is_action_just_pressed(name="ui_confirm"):
            GameState().mode = self.selected_game_mode
            SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")
