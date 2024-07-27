from crescent import Node2D, Input, Engine, Vector2, Sprite, Texture, Rect2


class Timer:
    def __init__(self, wait_time: float):
        self.wait_time = wait_time
        self.time = 0

    def tick(self, delta_time: float) -> bool:
        self.time += delta_time
        if self.time >= self.wait_time:
            return True
        return False


timer = Timer(1.0)


class Main(Node2D):
    def _start(self) -> None:
        Engine.set_fps_display_enabled(True)

        # TODO: Test perf
        # for i in range(1000):
        #     test_sprite = Sprite.new()
        #     test_sprite.texture = Texture("assets/images/crescent_logo.png")
        #     test_sprite.draw_source = Rect2(0.0, 0.0, 167.0, 74.0)
        #     test_sprite.position = Vector2(i, i)
        #     self.add_child(test_sprite)

    def _sprite_test(self, delta_time: float, speed=100) -> None:
        sprite = self.get_child("Sprite")
        if sprite:
            sprite.position += Vector2(speed, speed) * Vector2(delta_time, delta_time)
        sprite2 = self.get_child("Sprite2")
        if sprite2:
            sprite2.position += Vector2(-speed, speed) * Vector2(delta_time, delta_time)

    def _process(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("exit"):
            Engine.exit()

        # if timer.tick(delta_time):
        #     self._sprite_test(delta_time)

    def _fixed_process(self, delta_time: float) -> None:
        if timer.tick(delta_time):
            self._sprite_test(delta_time)
