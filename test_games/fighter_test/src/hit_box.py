from crescent_api import *
from test_games.fighter_test.src.task import *


class HitBox(Collider2D):
    pass


class Attack(HitBox):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 1.0

    def _start(self) -> None:
        self.set_extents(Size2D(32, 32))
        self.set_color(Color(200, 0, 0, 150))

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
