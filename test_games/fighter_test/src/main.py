from rbe_py_api import *


class Main(Node2D):
    def _start(self) -> None:
        self.velocity = Vector2.ZERO()
        self.speed = 50
        print(f"[PY SCRIPT] Entity entered stage with id = {self.entity_id}")
        Input.add_action(name="test", value=Input.Keyboard.RETURN)
        Input.add_action(name="move_left", value=Input.Keyboard.A)
        Input.add_action(name="move_right", value=Input.Keyboard.D)
        print(f"[PY SCRIPT] position = {self.get_position()}")
        Engine.set_fps_display_enabled(True)

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
            self.velocity += Vector2.LEFT()
        if Input.is_action_pressed(name="move_right"):
            self.velocity += Vector2.RIGHT()

    print(f"average fps = {Engine.get_average_fps()}")

    def _physics_update(self, delta_time: float) -> None:
        if self.velocity != Vector2.ZERO():
            # print(f"delta time = {delta_time}")
            delta_vector = Vector2(self.speed * delta_time, self.speed * delta_time)
            self.add_to_position(self.velocity * delta_vector)
            self.velocity = Vector2.ZERO()

    def _end(self) -> None:
        print(f"entity {self.entity_id} end was called!")
        pass
