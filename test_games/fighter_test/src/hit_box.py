from crescent_api import *


class HitBox(Collider2D):
    pass


class Attack(HitBox):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 1.0

    def _start(self) -> None:
        self.set_rect(Rect2(0, 0, 32, 32))
        self.set_color(Color(200, 0, 0, 150))

    def update_task(self, delta_time: float):
        current_time = 0.0
        try:
            while True:
                current_time += delta_time
                if current_time >= self.life_time:
                    break
                else:
                    yield True
        except GeneratorExit:
            pass
