from crescent_api import *


class Main(Node2D):
    def _start(self) -> None:
        pass

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()
