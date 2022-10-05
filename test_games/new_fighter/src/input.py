from crescent_api import *


class InputBuffer:
    """
    Takes action names as parameters to query for input
    """

    def __init__(
        self,
        move_left_action_name: str,
        move_right_action_name: str,
        crouch_action_name: str,
        jump_action_name: str,
        light_punch_action_name: str,
        heavy_punch_action_name: str,
        light_kick_action_name: str,
        heavy_kick_action_name: str,
    ):
        self._move_left = move_left_action_name
        self._move_right = move_right_action_name
        self._crouch = crouch_action_name
        self._jump = jump_action_name
        self._light_punch = light_punch_action_name
        self._heavy_punch = heavy_punch_action_name
        self._light_kick = light_kick_action_name
        self._heavy_kick = heavy_kick_action_name

        self.move_left_pressed = False
        self.move_right_pressed = False
        self.crouch_pressed = False
        self.jump_pressed = False
        self.light_punch_pressed = False
        self.heavy_punch_pressed = False
        self.light_kick_pressed = False
        self.heavy_kick_pressed = False

    def process_inputs(self) -> bool:
        self.move_left_pressed = Input.is_action_pressed(name=self._move_left)
        self.move_right_pressed = Input.is_action_pressed(name=self._move_right)
        self.crouch_pressed = Input.is_action_pressed(name=self._crouch)
        self.jump_pressed = Input.is_action_pressed(name=self._jump)
        self.light_punch_pressed = Input.is_action_pressed(name=self._light_punch)
        self.heavy_punch_pressed = Input.is_action_pressed(name=self._heavy_punch)
        self.light_kick_pressed = Input.is_action_pressed(name=self._light_kick)
        self.heavy_kick_pressed = Input.is_action_pressed(name=self._heavy_kick)
        return (
            self.move_left_pressed
            or self.move_right_pressed
            or self.crouch_pressed
            or self.jump_pressed
            or self.light_punch_pressed
            or self.heavy_punch_pressed
            or self.light_kick_pressed
            or self.heavy_kick_pressed
        )

    def kill_inputs(self) -> None:
        self.move_left_pressed = False
        self.move_right_pressed = False
        self.crouch_pressed = False
        self.jump_pressed = False
        self.light_punch_pressed = False
        self.heavy_punch_pressed = False
        self.light_kick_pressed = False
        self.heavy_kick_pressed = False

    def any_movement_pressed(self) -> bool:
        return (
            self.move_left_pressed
            or self.move_right_pressed
            or self.crouch_pressed
            or self.jump_pressed
        )

    def any_attack_pressed(self) -> bool:
        return (
            self.light_punch_pressed
            or self.heavy_punch_pressed
            or self.light_kick_pressed
            or self.heavy_kick_pressed
        )

    def is_input_pressed(
        self, input_name: str, facing_dir: Vector2 = Vector2.RIGHT()
    ) -> bool:
        if input_name == "forward":
            if facing_dir == Vector2.RIGHT():
                return self.move_right_pressed
            return self.move_left_pressed
        elif input_name == "backward":
            if facing_dir == Vector2.RIGHT():
                return self.move_left_pressed
            return self.move_right_pressed
        elif input_name == "down":
            return self.crouch_pressed
        elif input_name == "up":
            return self.jump_pressed
        return False
        # return locals().get(input_name, False)


class NetworkSenderInputBuffer(InputBuffer):
    def __init__(
        self,
        move_left_action_name: str,
        move_right_action_name: str,
        crouch_action_name: str,
        jump_action_name: str,
        light_punch_action_name: str,
        heavy_punch_action_name: str,
        light_kick_action_name: str,
        heavy_kick_action_name: str,
    ):
        super().__init__(
            move_left_action_name,
            move_right_action_name,
            crouch_action_name,
            jump_action_name,
            light_punch_action_name,
            heavy_punch_action_name,
            light_kick_action_name,
            heavy_kick_action_name,
        )
        self.is_server = True
        self.send_func = Server.send

    def process_inputs(self) -> bool:
        result = super().process_inputs()
        self.send_func(f"lm:{self.move_left_pressed},rm:{self.move_right_pressed}")
        return result


# TODO: Make receiver use prediction
class NetworkReceiverInputBuffer(InputBuffer):
    def process_inputs(self) -> bool:
        return False


# TODO: Implement AI
class AIInputBuffer(InputBuffer):
    def process_inputs(self) -> bool:
        return False
