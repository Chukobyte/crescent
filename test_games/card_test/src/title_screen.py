from crescent_api import *


class MouseTracker:
    def __init__(self):
        self.node_being_tracked = None
        self.node_offset = Vector2.ZERO()
        self.previous_mouse_pos = Vector2(-1, -1)

    def process_left_mouse_pressed(self) -> None:
        # Process Collisions
        collisions = CollisionHandler.process_mouse_collisions()
        for collision in collisions:
            collision_parent = collision.get_parent()
            if not self.node_being_tracked:
                self.node_being_tracked = collision_parent
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
        self.node_being_tracked = None


class TitleScreen(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.mouse_tracker = MouseTracker()

    def _start(self) -> None:
        pass

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("ui_back"):
            Engine.exit()

        if Input.is_action_pressed("lmb"):
            self.mouse_tracker.process_left_mouse_pressed()

        if Input.is_action_just_released("lmb"):
            self.mouse_tracker.process_left_mouse_released()
