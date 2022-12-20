import random

from crescent_api import *
from src.utils.task import TaskManager, co_suspend, Task, co_wait_seconds
from src.utils.timer import Timer


def get_player() -> Node2D:
    return SceneTree.get_root().get_child(name="Player")


class Enemy(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.direction_facing = Vector2.RIGHT()
        self.color_rect = None
        self.collider = None
        self.speed = 30

    def _physics_update(self, delta_time: float) -> None:
        # TODO: Look into world time dilation not being integrated into the delta time
        self.add_to_position(
            Vector2(
                self.speed
                * delta_time
                * self.direction_facing.x
                * World.get_time_dilation(),
                0,
            )
        )

    def _init_components(self, size: Size2D, color: Color = None) -> None:
        if not color:
            color = Color.linear_color(0.9, 0.9, 0.9)
        self.color_rect = ColorRect.new()
        self.color_rect.size = size
        self.color_rect.color = color
        self.add_child(self.color_rect)
        self.collider = Collider2D.new()
        self.collider.extents = size
        self.add_child(self.collider)

    def get_center_pos(self) -> Vector2:
        center_pos = self.position
        if self.color_rect:
            center_pos += self.color_rect.size.to_vec2() / Vector2(2, 2)
        return center_pos


class EnemyAttack(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.enemy: Type[Enemy, None] = None
        self.damage = 10
        self.speed = 40

    @staticmethod
    def spawn(attack_type: Type["EnemyAttack"], enemy: Enemy) -> "EnemyAttack":
        attack = attack_type.new()
        attack.enemy = enemy
        return attack


class GingerBreadMan(Enemy):
    def _start(self) -> None:
        self._init_components(
            size=Size2D(17, 17), color=Color.linear_color(0.8, 0.8, 0.1)
        )
        self.speed = 35


class Elf(Enemy):
    def _start(self) -> None:
        self._init_components(
            size=Size2D(4, 6), color=Color.linear_color(0.0, 0.7, 0.0)
        )


class SnowManAttack(EnemyAttack):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.color_rect = None
        self.collider = None
        self.direction = Vector2.ZERO()
        self.timer = Timer(5.0)

    def _start(self) -> None:
        size = Size2D(4, 4)
        # Color Rect
        self.color_rect = ColorRect.new()
        self.color_rect.color = Color.WHITE()
        self.color_rect.size = size
        self.add_child(self.color_rect)
        # Collider
        self.collider = Collider2D.new()
        self.collider.extents = size
        self.add_child(self.collider)

    def _physics_update(self, delta_time: float) -> None:
        self.add_to_position(
            Vector2(
                self.direction.x * self.speed * delta_time,
                self.direction.y * self.speed * delta_time,
            )
        )
        if self.timer.tick(delta_time).time_remaining <= 0.0:
            self.queue_deletion()


class SnowMan(Enemy):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self._physics_update_task_manager = TaskManager(
            tasks=[Task(self.physics_update_task())]
        )

    def _start(self) -> None:
        self._init_components(
            size=Size2D(17, 17), color=Color.linear_color(0.7, 0.7, 0.7)
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
            RETREATING = 2  # When player is too close, snow man will try to retreat

        def determine_state(snowman: SnowMan, player_node: Node2D) -> int:
            attack_range = MinMax(36, 40)
            distance_to_player = snowman.get_center_pos().distance_to(
                player_node.position + Vector2(-8, -8)
            )
            if attack_range.is_above(distance_to_player):
                return SnowManState.FOLLOWING_PLAYER
            elif attack_range.is_below(distance_to_player):
                return SnowManState.RETREATING
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
                elif current_state == SnowManState.RETREATING:
                    dir_to_player = self.position.direction_to(player.position)
                    if dir_to_player.x > 0:
                        self.direction_facing = Vector2.LEFT()
                    else:
                        self.direction_facing = Vector2.RIGHT()
                    self.add_to_position(
                        Vector2(self.speed * delta_time * self.direction_facing.x, 0)
                    )
                elif current_state == SnowManState.ATTACKING_PLAYER:
                    await co_wait_seconds(1.5)
                    # Attack
                    snowman_attack = EnemyAttack.spawn(SnowManAttack, self)
                    # High Attack
                    if random.choice([0, 1]) == 0:
                        attack_offset = Vector2(0, -4)
                    # Low attack
                    else:
                        attack_offset = Vector2(0, 3)
                    snowman_attack.position = self.get_center_pos() + attack_offset
                    dir_to_player = self.position.direction_to(player.position)
                    if dir_to_player.x > 0:
                        snowman_attack.direction = Vector2.RIGHT()
                    else:
                        snowman_attack.direction = Vector2.LEFT()
                    SceneTree.get_root().add_child(snowman_attack)
                    # Delay after attack
                    await co_wait_seconds(1.0)
                await co_suspend()
        except GeneratorExit:
            pass
