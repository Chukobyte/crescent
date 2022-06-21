from rbe_py_api import *
from test_games.fighter_test.src.game_state import *


class TitleScreen(Node2D):
    def _start(self) -> None:
        self.selected_game_mode = GameMode.LOCAL_PVP

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

        if Input.is_action_just_pressed(name="ui_confirm"):
            GameState().mode = self.selected_game_mode
            SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")
