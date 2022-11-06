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
        if not self.node_being_tracked:
            collisions = CollisionHandler.process_mouse_collisions()
            game_state = GameState()
            for collision in collisions:
                collision_parent = collision.get_parent()
                collision_parent_name = collision_parent.name
                print(f"collision_parent_name = {collision_parent_name}")
                # Try card
                card = game_state.get_card_data(collision_parent_name)
                if card:
                    print(
                        f"card: name='{collision_parent_name}', is_in_hand='{card.is_in_hand}'"
                    )
                    if not card.is_in_hand:
                        self.node_being_tracked = collision_parent
                        self.initial_position = collision_parent.global_position
                        mouse_world_pos = Input.Mouse.get_world_position()
                        self.node_offset = (
                            self.node_being_tracked.position - mouse_world_pos
                        )
                        print(f"Tracking new node {self.node_being_tracked}")
                        card.is_in_hand = True
                        game_state.can_end_turn = True
                        break
                # Try end turn
                elif collision_parent_name == "EndTurnButton":
                    if game_state.end_turn():
                        break

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
