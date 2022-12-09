from crescent_api import *
from task import *


class Attack(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 1.0
        self.targets = []
        self.direction = Vector2.RIGHT()
        self._task_manager = TaskManager([Task(self._update_task())])

    def _start(self) -> None:
        collider_size = Size2D(32, 32)
        collider_color = Color(0, 200, 0, 150)
        self.set_extents(collider_size)
        self.set_color(collider_color)
        color_square = ColorRect.new()
        color_square.size = collider_size
        color_square.color = collider_color
        self.add_child(color_square)

    def _update(self, delta_time: float) -> None:
        self._task_manager.update()

    async def _update_task(self) -> None:
        try:
            while True:
                collisions = CollisionHandler.process_collisions(self)
                for collision in collisions:
                    if collision.get_parent().name == "TestThing":
                        collision.get_parent().queue_deletion()
                        self.queue_deletion()
                        break
                await co_suspend()
        except GeneratorExit:
            pass


class Player(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.color_rect = None
        self.collider = None
        self.speed = 200
        self.direction_facing = Vector2.RIGHT()

    def _start(self) -> None:
        self.color_rect = self.get_child("ColorRect")
        self.collider = self.get_child("Collider2D")
        Camera2D.follow_node(node=self)

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

        if Input.is_action_just_pressed(name="attack"):
            new_attack = Attack.new()
            new_attack.position = (
                self.position
                + Vector2(24, 0)
                + Vector2(self.direction_facing.x * 10, 20)
            )
            self.get_parent().add_child(new_attack)
            print("Attacked")

    def _physics_update(self, delta_time: float) -> None:
        if Input.is_action_pressed(name="move_left"):
            self.add_to_position(
                Vector2.LEFT()
                * Vector2(delta_time * self.speed, delta_time * self.speed)
            )
            self.direction_facing = Vector2.LEFT()
        elif Input.is_action_pressed(name="move_right"):
            self.add_to_position(
                Vector2.RIGHT()
                * Vector2(delta_time * self.speed, delta_time * self.speed)
            )
            self.direction_facing = Vector2.RIGHT()
