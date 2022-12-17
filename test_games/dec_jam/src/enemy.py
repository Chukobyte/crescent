from crescent_api import Node2D, ColorRect, Collider2D, Vector2, Size2D, Color


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
