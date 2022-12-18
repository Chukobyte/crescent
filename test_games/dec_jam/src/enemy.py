from crescent_api import *
from src.utils.task import TaskManager, co_suspend, Task, co_wait_seconds


def get_player() -> Node2D:
    return SceneTree.get_root().get_child(name="Player")


class Enemy(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.direction_facing = Vector2.RIGHT()
        self.speed = 100

    def _physics_update(self, delta_time: float) -> None:
        self.add_to_position(
            Vector2(self.speed * delta_time * self.direction_facing.x, 0)
        )

    def _init_components(self, size: Size2D, color: Color = None) -> None:
        if not color:
            color = Color.linear_color(0.9, 0.9, 0.9)
        color_rect = ColorRect.new()
        color_rect.size = size
        color_rect.color = color
        self.add_child(color_rect)
        collider_2d = Collider2D.new()
        collider_2d.extents = size
        self.add_child(collider_2d)


class GingerBreadMan(Enemy):
    def _start(self) -> None:
        self._init_components(
            size=Size2D(48, 64), color=Color.linear_color(0.8, 0.8, 0.1)
        )


class Elf(Enemy):
    def _start(self) -> None:
        self._init_components(
            size=Size2D(48, 32), color=Color.linear_color(0.0, 0.7, 0.0)
        )


class SnowMan(Enemy):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self._physics_update_task_manager = TaskManager(
            tasks=[Task(self.physics_update_task())]
        )

    def _start(self) -> None:
        self._init_components(
            size=Size2D(48, 64), color=Color.linear_color(0.7, 0.7, 0.7)
        )

    def _end(self) -> None:
        self._physics_update_task_manager.kill_tasks()

    def _physics_update(self, delta_time: float) -> None:
        self._physics_update_task_manager.update()

    async def physics_update_task(self) -> None:
        # Snowman state
        class SnowManState:
            FOLLOWING_PLAYER = 0  # Will follow player if the player is far away
            ATTACKING_PLAYER = 1  # When within attack range
            RETREATING_FROM_PLAYER = (
                2  # When player is too close, snow man will try to retreat
            )

        def determine_state(snowman: Node2D, player_node: Node2D) -> int:
            attack_range = MinMax(64, 128)
            distance_to_player = player.position.distance_to(self.position)
            if attack_range.is_above(distance_to_player):
                return SnowManState.FOLLOWING_PLAYER
            elif attack_range.is_below(attack_range.min):
                return SnowManState.RETREATING_FROM_PLAYER
            else:
                return SnowManState.ATTACKING_PLAYER

        delta_time = Engine.get_global_physics_delta_time()
        current_state = SnowManState.FOLLOWING_PLAYER
        player = get_player()
        try:
            while True:
                # Determine state
                current_state = determine_state(self, player)
                if current_state == SnowManState.FOLLOWING_PLAYER:
                    dir_to_player = self.position.direction_to(player.position)
                    if dir_to_player.x > 0:
                        self.direction_facing = Vector2.RIGHT()
                    else:
                        self.direction_facing = Vector2.LEFT()
                    self.add_to_position(
                        Vector2(self.speed * delta_time * self.direction_facing.x, 0)
                    )
                elif current_state == SnowManState.RETREATING_FROM_PLAYER:
                    dir_to_player = self.position.direction_to(player.position)
                    if dir_to_player.x > 0:
                        self.direction_facing = Vector2.LEFT()
                    else:
                        self.direction_facing = Vector2.RIGHT()
                    self.add_to_position(
                        Vector2(self.speed * delta_time * self.direction_facing.x, 0)
                    )
                elif current_state == SnowManState.ATTACKING_PLAYER:
                    await co_wait_seconds(1.0)
                    # Attack
                    # Delay after attack
                    await co_wait_seconds(0.5)
                await co_suspend()
        except GeneratorExit:
            pass
