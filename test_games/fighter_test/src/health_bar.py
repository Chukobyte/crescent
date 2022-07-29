from crescent_api import *


HEALTH_BAR_SIZE = Size2D(100, 20)


class HealthBar(ColorSquare):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.inner_hp_bar = ColorSquare.new()

    def _start(self) -> None:
        self.size = HEALTH_BAR_SIZE
        self.color = Color(0, 0, 0)

        self.inner_hp_bar.position = Vector2(1, 1)
        self.inner_hp_bar.size = Size2D(HEALTH_BAR_SIZE.w - 2, HEALTH_BAR_SIZE.h - 2)
        self.inner_hp_bar.color = Color(255, 0, 0)
        self.add_child(self.inner_hp_bar)

    def set_health_percentage(self, hp_percent: int) -> None:
        percent_decimal = hp_percent / 100.0
        new_bar_width = HEALTH_BAR_SIZE.w * percent_decimal
        self.inner_hp_bar.size = Size2D(new_bar_width, HEALTH_BAR_SIZE.h)
