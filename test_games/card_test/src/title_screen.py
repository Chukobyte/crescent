from crescent_api import *
from src.game_state import GameState
from src.mouse_tracker import MouseTracker


class TitleScreen(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.mouse_tracker = MouseTracker()

    def _start(self) -> None:
        game_state = GameState()
        game_state.initialize(self)

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("ui_back"):
            Engine.exit()

        if Input.is_action_pressed("lmb"):
            self.mouse_tracker.process_left_mouse_pressed()

        if Input.is_action_just_released("lmb"):
            self.mouse_tracker.process_left_mouse_released()
