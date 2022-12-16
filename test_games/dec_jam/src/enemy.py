from crescent_api import Node2D, ColorRect, Collider2D, Size2D, Color


class Enemy(Node2D):
    pass


class GingerBreadMan(Enemy):
    def _start(self) -> None:
        size = Size2D(48, 64)
        color_rect = ColorRect.new()
        color_rect.size = size
        color_rect.color = Color.linear_color(0.9, 0.9, 0.9)
        self.add_child(color_rect)
        collider_2d = Collider2D.new()
        collider_2d.extents = size
        self.add_child(collider_2d)
