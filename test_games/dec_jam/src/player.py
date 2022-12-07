from crescent_api import *


class Player(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.color_rect = None
        self.collider = None
        self.speed = 200

    def _start(self) -> None:
        self.color_rect = self.get_child("ColorRect")
        self.collider = self.get_child("Collider2D")
        print("Player Start")

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

    def _physics_update(self, delta_time: float) -> None:
        if Input.is_action_pressed(name="move_left"):
            self.add_to_position(Vector2.LEFT() * Vector2(delta_time * self.speed, delta_time * self.speed))
        elif Input.is_action_pressed(name="move_right"):
            self.add_to_position(Vector2.RIGHT() * Vector2(delta_time * self.speed, delta_time * self.speed))
