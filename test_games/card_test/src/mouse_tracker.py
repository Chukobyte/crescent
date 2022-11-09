from crescent_api import *
from src.card import Card
from src.game_state import GameState


class MouseTracker:
    def __init__(self):
        self.node_being_tracked = None
        self.node_offset = Vector2.ZERO()
        self.previous_mouse_pos = Vector2(-1, -1)
        self.initial_position = Vector2(0, 0)

    def process_left_mouse_pressed(self) -> None:
        # Process Collisions
        if not self.node_being_tracked:
            collisions = CollisionHandler.process_mouse_collisions()
            game_state = GameState()
            for collision in collisions:
                collision_parent = collision.get_parent()
                collision_parent_name = collision_parent.name
                # Try card
                if isinstance(collision_parent, Card):
                    if collision_parent.location == Card.Location.Hand:
                        print(f"picking up card_id = {collision_parent.id}")
                        collision_parent.location = Card.Location.Moving
                        self.node_being_tracked = collision_parent
                        self.initial_position = collision_parent.global_position
                        mouse_world_pos = Input.Mouse.get_world_position()
                        self.node_offset = (
                            self.node_being_tracked.position - mouse_world_pos
                        )
                        game_state.can_end_turn = True
                # Try end turn
                elif collision_parent_name == "EndTurnButton":
                    if game_state.end_turn():
                        break

        # Process movement
        if self.node_being_tracked:
            mouse_pos = Input.Mouse.get_world_position()
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
                    has_landed_in_card_zone = True
                    if isinstance(self.node_being_tracked, Card):
                        card_zone.player_one_score += self.node_being_tracked.power
                        self.node_being_tracked.location = Card.Location.Zone
                    break
            if not has_landed_in_card_zone:
                self.node_being_tracked.position = self.initial_position
                if isinstance(self.node_being_tracked, Card):
                    self.node_being_tracked.location = Card.Location.Hand
            self.node_being_tracked = None
