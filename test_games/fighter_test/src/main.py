from rbe_py_api import *


class Main(Node2D):
    def _start(self) -> None:
        self.velocity = Vector2.ZERO()
        self.speed = 50
        print(f"[PYTHON SCRIPT] Entity entered stage with id = {self.entity_id}")
        Input.add_action(name="test", value=Input.Keyboard.RETURN)
        Input.add_action(name="move_left", value=Input.Keyboard.A)
        Input.add_action(name="move_right", value=Input.Keyboard.D)
        print(f"[PYTHON SCRIPT] position = {self.get_position()}")
        Engine.set_fps_display_enabled(True)

        # Network
        self.is_server = False

        if self.is_server:
            Server.start(port=8888)
            Server.subscribe(
                signal_id="data_callback",
                listener_node=self,
                listener_func=self._network_server_callback,
            )
        else:
            Client.start("127.0.0.1", 8888)
            Client.subscribe(
                signal_id="data_callback",
                listener_node=self,
                listener_func=self._network_client_callback,
            )
        print(f"[PYTHON SCRIPT] Is server = {self.is_server}")

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

    # print(f"average fps = {Engine.get_average_fps()}")

    def _physics_update(self, delta_time: float) -> None:
        if self.velocity != Vector2.ZERO():
            # print(f"delta time = {delta_time}")
            delta_vector = Vector2(self.speed * delta_time, self.speed * delta_time)
            self.add_to_position(self.velocity * delta_vector)
            self.velocity = Vector2.ZERO()

    def _end(self) -> None:
        print(f"entity {self.entity_id} end was called!")
        pass

    def _network_server_callback(self, message: str) -> None:
        print(f"[PYTHON SCRIPT] [SERVER] _network_server_callback - message: {message}")

    def _network_client_callback(self, message: str) -> None:
        print(f"[PYTHON SCRIPT] [CLIENT] _network_client_callback - message: {message}")
