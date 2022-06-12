from rbe_py_api import *


class Main(Node2D):
    def _start(self) -> None:
        print(f"[SCRIPT] Entity entered stage with id = {self.entity_id}")
        Input.add_action(name="test", value=Input.Keyboard.RETURN)

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()
        if Input.is_action_just_pressed(name="test"):
            SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")


    def _end(self) -> None:
        pass
