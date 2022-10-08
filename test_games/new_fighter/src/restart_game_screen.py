from crescent_api import *


class RestartGameScreen(Node2D):
    def _start(self) -> None:
        pass

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

        if Input.is_action_just_pressed(name="ui_confirm"):
            # SceneTree.change_scene(path="nodes/main.cscn")
            pass
