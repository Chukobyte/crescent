from typing import List, Optional, Callable
import json

from crescent_api import Vector2
from src.input import InputBuffer
from src.timer import Timer

TEMP_GLOBAL_DELTA = 0.1


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
        new_move.timer = Timer(new_move.time_window)
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


# Special Moves Manager
class SpecialMoveTask:
    def __init__(self, move: SpecialMove):
        self.move = move
        self.is_active = False
        self.move_index = 0
        self.move_timer = Timer(move.time_window)

    def update(self) -> None:
        pass

    def reset(self) -> None:
        self.is_active = False
        self.move_index = 0
        self.move_timer.reset()

    def does_current_move_satisfy_criteria(
        self, input_buffer: InputBuffer, facing_dir: Vector2
    ) -> bool:
        for value in self.move.commands[self.move_index].values:
            if not input_buffer.is_input_pressed(
                input_name=value, facing_dir=facing_dir
            ):
                return False
        return True

    def has_executed_all_commands(self) -> bool:
        return self.move_index >= len(self.move.commands)


class SpecialMovesManager:
    def __init__(
        self,
        moves: Optional[List[SpecialMove]] = None,
        on_completed_funcs: List[Callable] = None,
    ):
        self.moves = []
        self.tasks = []
        if on_completed_funcs:
            self.on_completed_funcs = on_completed_funcs
        else:
            self.on_completed_funcs = []
        if moves:
            for move in moves:
                self.add_move(move)

    def has_move_triggered(self, name: str) -> bool:
        return False

    def add_move(self, move: SpecialMove) -> None:
        self.moves.append(move)
        self.tasks.append(SpecialMoveTask(move))

    # TODO: Return handle
    def bind_on_completed_func(self, func: Callable) -> None:
        self.on_completed_funcs.append(func)

    def add_moves_from_file(self, file_path: str):
        json_file = open(file_path)
        json_data = json.load(json_file)
        for move_data in json_data["moves"]:
            self.add_move(SpecialMove.from_json(move_data))
        json_file.close()

    def reset_all_task_states(self) -> None:
        for task in self.tasks:
            task.reset()

    def _process_and_broadcast_completed_move(self, task: SpecialMoveTask) -> None:
        for func in self.on_completed_funcs:
            func(task.move)
        self.reset_all_task_states()

    def update(self, input_buffer: InputBuffer, facing_dir: Vector2) -> None:
        for task in self.tasks:
            # Update current move task
            if task.is_active:
                # Timeout
                if task.move_timer.tick(TEMP_GLOBAL_DELTA):
                    task.reset()
                # In progress
                else:
                    if task.does_current_move_satisfy_criteria(
                        input_buffer, facing_dir
                    ):
                        task.move_index += 1
                        if task.has_executed_all_commands():
                            self._process_and_broadcast_completed_move(task)
                            break
            # Attempt to start move task
            else:
                if task.does_current_move_satisfy_criteria(input_buffer, facing_dir):
                    task.is_active = True
                    task.move_index += 1
                    if task.has_executed_all_commands():
                        self._process_and_broadcast_completed_move(task)
                        break
