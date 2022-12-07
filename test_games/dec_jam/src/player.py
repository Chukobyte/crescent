from crescent_api import *


class Player(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.color_rect = None
        self.collider = None

    def _start(self) -> None:
        self.color_rect = self.get_child("ColorRect")
        self.collider = self.get_child("Collider2D")
        print("Player Start")

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()
