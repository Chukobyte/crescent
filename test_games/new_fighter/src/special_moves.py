from typing import List

# from abc import ABC, abstractmethod

from src.input import InputBuffer
from src.timer import Timer

TEMP_GLOBAL_DELTA = 0.1


# class Command(ABC):
#     @abstractmethod
#     def has_triggered(self) -> bool:
#         return False
#
#
# class PressedCommand(Command):
#     def __init__(
#         self, input_buffer: InputBuffer, input_names: List[str], time_window=None
#     ):
#         self.input_buffer = input_buffer
#         self.input_names = input_names
#         self.time_window = time_window
#
#     def has_triggered(self) -> bool:
#         triggered = False
#         for input_name in self.input_names:
#             if self.input_buffer.is_input_pressed(input_name):
#                 triggered = True
#             else:
#                 return False
#         return triggered
#
#
# class ReleasedCommand(Command):
#     def __init__(self, input_buffer: InputBuffer, input_names: List[str]):
#         self.input_buffer = input_buffer
#         self.input_names = input_names
#
#     def has_triggered(self) -> bool:
#         triggered = False
#         for input_name in self.input_names:
#             if self.input_buffer.is_input_pressed(input_name):
#                 triggered = True
#             else:
#                 return False
#         return triggered


class MoveCommand:
    def __init__(self, pressed=True, values=None):
        if values is None:
            values = []
        self.pressed = pressed
        self.values = values

    @staticmethod
    def from_json(json_data: dict):
        new_command = MoveCommand()
        new_command.pressed = json_data.get("type", "pressed") == "pressed"
        new_command.values = json_data["values"]
        return new_command

    def has_triggered(self) -> bool:
        return False

    def __str__(self):
        return f"MoveCommand(\n  pressed: {self.pressed},\n  values: {self.values}\n)"

    def __repr__(self):
        return f"MoveCommand(\n  pressed: {self.pressed},\n  values: {self.values}\n)"


class SpecialMove:
    def __init__(self, name: str, commands: List[MoveCommand], time_window: float):
        self.name = name
        self.commands = commands
        self.time_window = time_window
        self._command_index = 0
        self.timer = Timer(time_window)

    @staticmethod
    def from_json(json_data: dict):
        new_move = SpecialMove("temp", [], 0.0)
        new_move.name = json_data["name"]
        new_move.time_window = json_data["time_window"]
        for command in json_data["commands"]:
            new_move.commands.append(MoveCommand.from_json(command))
        return new_move

    def has_triggered(self) -> bool:
        if self.commands[self._command_index].has_triggered():
            if self._command_index > len(self.commands) - 1:
                pass
        return False

    def _get_commands_text(self) -> str:
        text = "["
        if self.commands:
            text += "\n"
            for command in self.commands:
                text += f"    MoveCommand(\n      pressed: {command.pressed},\n      values: {command.values}\n    ),\n"
        text += "  ],"
        return text

    def __str__(self):
        return f"SpecialMove(\n  name: {self.name},\n  time_window: {self.time_window},\n  commands: {self._get_commands_text()}\n)"

    def __repr__(self):
        return f"SpecialMove(\n  name: {self.name},\n  time_window: {self.time_window},\n  commands: {self._get_commands_text()}\n)"


class SpecialMovesManager:
    def __init__(self, moves: List[SpecialMove]):
        self.moves = moves

    def has_move_triggered(self, name: str) -> bool:
        return False

    def add_move(self, move: SpecialMove) -> None:
        self.moves.append(move)

    def update(self) -> None:
        pass
