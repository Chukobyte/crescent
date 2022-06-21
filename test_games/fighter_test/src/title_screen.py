from rbe_py_api import *


class TitleScreen(Node2D):
    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

        if Input.is_action_just_pressed(name="ui_confirm"):
            SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")
