from crescent_api import *


HEALTH_BAR_SIZE = Size2D(100, 20)
INNER_HEALTH_BAR_SIZE = HEALTH_BAR_SIZE - Size2D(2, 2)


class HealthBar(ColorRect):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.inner_hp_bar = ColorRect.new()

    def _start(self) -> None:
        self.size = HEALTH_BAR_SIZE
        self.color = Color.BLACK()

        self.inner_hp_bar.position = Vector2(1, 1)
        self.inner_hp_bar.size = INNER_HEALTH_BAR_SIZE
        self.inner_hp_bar.color = Color.RED()
        self.add_child(self.inner_hp_bar)

    def set_health_percentage(self, hp_percent: int) -> None:
        percent_decimal = hp_percent / 100.0
        new_bar_width = (HEALTH_BAR_SIZE.w * percent_decimal) - 2
        self.inner_hp_bar.size = Size2D(new_bar_width, INNER_HEALTH_BAR_SIZE.h)

    def print_debug_info(self) -> None:
        print(
            f"size = {self.size}, color = {self.color}\ninner_size = {self.inner_hp_bar.size}, inner_color = {self.inner_hp_bar.color}"
        )
