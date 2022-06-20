from rbe_py_api import *
# from src.input import InputBuffer
from test_games.fighter_test.src.input import InputBuffer


class Main(Node2D):
    def _start(self) -> None:
        self.velocity = Vector2.ZERO()
        self.speed = 50
        print(f"[PYTHON SCRIPT] Entity entered stage with id = {self.entity_id}")
        Input.add_action(name="test", value=Input.Keyboard.RETURN)
        Input.add_action(name="p1_move_left", value=Input.Keyboard.A)
        Input.add_action(name="p1_move_right", value=Input.Keyboard.D)
        Input.add_action(name="p2_move_left", value=Input.Keyboard.LEFT)
        Input.add_action(name="p2_move_right", value=Input.Keyboard.RIGHT)
        print(f"[PYTHON SCRIPT] position = {self.get_position()}")
        Engine.set_fps_display_enabled(True)

        # Input Buffer (Will move into server for player 2 soon)
        self.player_one_input = InputBuffer(
            move_left_action_name="p1_move_left", move_right_action_name="p1_move_right"
        )
        self.player_two_input = InputBuffer(
            move_left_action_name="p2_move_left", move_right_action_name="p2_move_right"
        )

        # Network
        self.is_server = True

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

        # Player One (for now)
        self.player_one_input.process_inputs()
        if self.player_one_input.move_left_pressed:
            self.velocity += Vector2.LEFT()
        if self.player_one_input.move_right_pressed:
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

    def _network_server_callback(self, message: str) -> None:
        print(
            f"[PYTHON SCRIPT] [SERVER] _network_server_callback - message: '{message}'"
        )
        Server.send(message=message)

    def _network_client_callback(self, message: str) -> None:
        print(
            f"[PYTHON SCRIPT] [CLIENT] _network_client_callback - message: '{message}'"
        )
