from rbe_py_api import *
from test_games.fighter_test.src.input import InputBuffer

# from src.input import InputBuffer # TODO: Replace once path is overridden...


class Fighter:
    def __init__(self, node: Node, input_buffer: InputBuffer):
        self.node = node
        self.input_buffer = input_buffer


class Main(Node2D):
    def _start(self) -> None:
        self.velocity = Vector2.ZERO()
        self.speed = 50
        print(f"[PYTHON SCRIPT] Entity entered stage with id = {self.entity_id}")
        Input.add_action(name="p1_move_left", value=Input.Keyboard.A)
        Input.add_action(name="p1_move_right", value=Input.Keyboard.D)
        Input.add_action(name="p2_move_left", value=Input.Keyboard.LEFT)
        Input.add_action(name="p2_move_right", value=Input.Keyboard.RIGHT)
        Engine.set_fps_display_enabled(True)

        # Fighter Data
        # Nodes
        player_one_node = self.get_child(name="PlayerOne")
        player_two_node = self.get_child(name="PlayerTwo")
        print(f"[PYTHON_SCRIPT] p1 = {player_one_node}, p2 = {player_two_node}")

        # # Input Buffer (Will move into server for player 2 soon)
        # player_one_input = InputBuffer(
        #     move_left_action_name="p1_move_left", move_right_action_name="p1_move_right"
        # )
        # player_two_input = InputBuffer(
        #     move_left_action_name="p2_move_left", move_right_action_name="p2_move_right"
        # )
        # # Fighters
        # self.player_one_fighter = Fighter(player_one_node, player_one_input)
        # self.player_two_fighter = Fighter(player_two_node, player_two_input)

        # Network
        # self.is_server = True
        #
        # if self.is_server:
        #     Server.start(port=8888)
        #     Server.subscribe(
        #         signal_id="data_callback",
        #         listener_node=self,
        #         listener_func=self._network_server_callback,
        #     )
        # else:
        #     Client.start("127.0.0.1", 8888)
        #     Client.subscribe(
        #         signal_id="data_callback",
        #         listener_node=self,
        #         listener_func=self._network_client_callback,
        #     )
        # print(f"[PYTHON SCRIPT] Is server = {self.is_server}")

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="exit"):
            Engine.exit()

    def _physics_update(self, delta_time: float) -> None:
        if self.velocity != Vector2.ZERO():
            # print(f"delta time = {delta_time}")
            delta_vector = Vector2(self.speed * delta_time, self.speed * delta_time)
            self.add_to_position(self.velocity * delta_vector)
            self.velocity = Vector2.ZERO()

    def _network_server_callback(self, message: str) -> None:
        print(
            f"[PYTHON SCRIPT] [SERVER] _network_server_callback - message: '{message}'"
        )
        Server.send(message=message)

    def _network_client_callback(self, message: str) -> None:
        print(
            f"[PYTHON SCRIPT] [CLIENT] _network_client_callback - message: '{message}'"
        )
