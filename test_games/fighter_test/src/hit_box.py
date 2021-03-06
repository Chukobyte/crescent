from crescent_api import *
from test_games.fighter_test.src.task import *


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

    def _start(self) -> None:
        collider_size = Size2D(32, 32)
        collider_color = Color(200, 0, 0, 150)
        self.set_extents(collider_size)
        self.set_color(collider_color)
        color_square = ColorSquare.new()
        color_square.size = collider_size
        color_square.color = collider_color
        self.add_child(color_square)

    async def update_task(self, delta_time: float):
        current_time = 0.0
        try:
            while True:
                current_time += delta_time
                if current_time >= self.life_time:
                    break
                else:
                    await co_suspend()
        except GeneratorExit:
            pass
