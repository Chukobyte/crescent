from rbe_py_api import *


class Main(Node2D):
    def _start(self) -> None:
        print(f"[SCRIPT] Entity entered stage with id = {self.entity_id}")
        Input.add_action(name="test", value=Input.Keyboard.RETURN)
        Input.add_action(name="move_left", value=Input.Keyboard.A)
        Input.add_action(name="move_right", value=Input.Keyboard.D)
        print(f"[SCRIPT] position = {self.get_position()}")

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()
        if Input.is_action_just_pressed(name="test"):
            SceneTree.change_scene(path="test_games/fighter_test/nodes/main_node.py")
            AudioManager.stop_sound(
                path="test_games/fighter_test/assets/audio/sfx/rainbow_orb.wav"
            )
        if Input.is_action_just_pressed(name="play_sfx"):
            AudioManager.play_sound(
                path="test_games/fighter_test/assets/audio/sfx/rainbow_orb.wav",
                loops=False,
            )
        if Input.is_action_pressed(name="move_left"):
            print(f"entity_id = {self.entity_id}")
            self.add_to_position(Vector2(-1, 0))
        if Input.is_action_pressed(name="move_right"):
            print(f"entity_id = {self.entity_id}")
            self.add_to_position(Vector2(1, 0))

        # print(f"average fps = {Engine.get_average_fps()}")

    # def _physics_update(self, delta_time: float) -> None:
    #     pass

    def _end(self) -> None:
        print(f"entity {self.entity_id} end was called!")
        pass
