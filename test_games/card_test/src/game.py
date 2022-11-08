from crescent_api import *
from src.game_state import GameState
from src.mouse_tracker import MouseTracker
from src.card import Card, CardData


class Game(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.mouse_tracker = MouseTracker()

    def _start(self) -> None:
        game_state = GameState()
        game_state.initialize(self)
        # Card test
        card_test = Card.generate(CardData.ID.CardOne)
        card_test.position = Vector2(360, 440)
        card_test.z_index = 1
        card_test.location = Card.Location.Hand
        self.add_child(card_test)
        print(f"card_test = {card_test}")

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("ui_back"):
            Engine.exit()

        # All gameplay logic is centralized in mouse tracker for now...
        if Input.is_action_pressed("lmb"):
            self.mouse_tracker.process_left_mouse_pressed()

        if Input.is_action_just_released("lmb"):
            self.mouse_tracker.process_left_mouse_released()

        if Input.is_action_pressed("rmb"):
            print(
                f"mouse_position = {Input.Mouse.get_position()}, mouse_world_position = {Input.Mouse.get_world_position()}"
            )
