from crescent_api import *

from src.task import *


class HitBox(Collider2D):
    pass


class AttackType:
    HIGH = 0
    LOW = 1
    AIR = 2


class Attack(HitBox):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 1.0
        self.targets = []

    def _start(self) -> None:
        collider_size = Size2D(32, 32)
        collider_color = Color(200, 0, 0, 150)
        self.set_extents(collider_size)
        self.set_color(collider_color)
        color_square = ColorRect.new()
        color_square.size = collider_size
        color_square.color = collider_color
        self.add_child(color_square)

    def add_fighter_target(self, target) -> None:
        self.targets.append(target)

    def _is_entity_in_targets(self, node) -> bool:
        for fighter in self.targets:
            if node == fighter.node:
                return True
        return False

    async def update_task(self, delta_time: float):
        current_time = 0.0
        await co_suspend()  # FIXME: Need to delay because collision are checked right away

        try:
            while True:
                current_time += delta_time
                # Check collisions
                collided_entities = CollisionHandler.process_collisions(self)
                for node in collided_entities:
                    for fighter in self.targets:
                        if node == fighter.collider:
                            fighter.on_attack_connect(self)
                            raise GeneratorExit

                if current_time >= self.life_time:
                    break
                else:
                    await co_suspend()
        except GeneratorExit:
            pass
