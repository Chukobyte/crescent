from crescent import Node2D, Input, Engine, TextLabel, Vector2, Particles2D, Engine, MinMaxVector2


class Main(Node2D):
    def _start(self) -> None:
        Engine.set_fps_display_enabled(True)

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

        self._sprite_test(delta_time)

    def _fixed_process(self, delta_time: float) -> None:
        # self._sprite_test(delta_time)
        pass
