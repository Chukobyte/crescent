from crescent_api import *
from src.fight_sim.fight_sim import WinState
from src.game_state import GameState


class RestartGameScreen(Node2D):
    def _start(self) -> None:
        self.game_state = GameState()
        winner_text_label = self.get_child(name="WinnerText")
        if self.game_state.last_win_state == WinState.PLAYER_ONE:
            winner_text_label.text = "Player One Wins!"
        elif self.game_state.last_win_state == WinState.PLAYER_TWO:
            winner_text_label.text = "Player Two Wins!"

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

        if Input.is_action_just_pressed(name="ui_confirm"):
            self.game_state.last_win_state = WinState.NONE
            SceneTree.change_scene(path="nodes/main.cscn")
