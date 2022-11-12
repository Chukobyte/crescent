from crescent_api import *


class TitleScreen(Node2D):
    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("ui_back"):
            Engine.exit()

        if Input.is_action_just_pressed("lmb"):
            SceneTree.change_scene("scenes/game.cscn")
