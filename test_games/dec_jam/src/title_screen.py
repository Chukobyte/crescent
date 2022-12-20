from crescent_api import Node2D, Input, Engine, SceneTree


class TitleScreen(Node2D):
    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

        if Input.is_action_just_pressed(name="attack"):
            SceneTree.change_scene("scenes/main.cscn")
