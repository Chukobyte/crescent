from crescent_api import *
from src.game_state import GameState


class MouseTracker:
    def __init__(self):
        self.node_being_tracked = None
        self.node_offset = Vector2.ZERO()
        self.previous_mouse_pos = Vector2(-1, -1)
        self.initial_position = Vector2(0, 0)

    def process_left_mouse_pressed(self) -> None:
        # Process Collisions
        collisions = CollisionHandler.process_mouse_collisions()
        for collision in collisions:
            collision_parent = collision.get_parent()
            if collision_parent.name == "Card0" and not self.node_being_tracked:
                self.node_being_tracked = collision_parent
                self.initial_position = collision_parent.global_position
                mouse_world_pos = Input.Mouse.get_world_position()
                self.node_offset = self.node_being_tracked.position - mouse_world_pos
                print(f"Tracking new node {self.node_being_tracked}")
        # Process movement
        if self.node_being_tracked:
            mouse_pos = Input.Mouse.get_position()
            if mouse_pos != self.previous_mouse_pos:
                self.node_being_tracked.position = mouse_pos + self.node_offset
            self.previous_mouse_pos = mouse_pos

    def process_left_mouse_released(self) -> None:
        if self.node_being_tracked:
            has_landed_in_card_zone = False
            collisions = CollisionHandler.process_collisions(
                self.node_being_tracked.get_child("Collider2D")
            )
            for collision in collisions:
                collision_parent = collision.get_parent()
                card_zone = GameState().get_card_zone_data(collision_parent.name)
                if card_zone:
                    card_zone.player_one_score += 1
                    has_landed_in_card_zone = True
                    break
            if not has_landed_in_card_zone:
                self.node_being_tracked.position = self.initial_position
            self.node_being_tracked = None
