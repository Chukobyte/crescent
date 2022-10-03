from typing import List
from abc import ABC, abstractmethod

from src.input import InputBuffer


class Command(ABC):
    @abstractmethod
    def has_triggered(self) -> bool:
        return False


class PressedCommand(Command):
    def __init__(
        self, input_buffer: InputBuffer, input_names: List[str], time_window=None
    ):
        self.input_buffer = input_buffer
        self.input_names = input_names
        self.time_window = time_window


class ReleasedCommand(Command):
    def __init__(self, input_buffer: InputBuffer, input_names: List[str]):
        self.input_buffer = input_buffer
        self.input_names = input_names


class SpecialMove:
    def __init__(self, name: str, commands: List[Command], time_window: float):
        self.name = name
        self.commands = commands
        self.time_window = time_window

    def has_triggered(self) -> bool:
        return False


class SpecialMovesManager:
    def __init__(self, moves: List[SpecialMove]):
        self.moves = moves

    def has_move_triggered(self, name: str) -> bool:
        return False

    def add_move(self, move: SpecialMove) -> None:
        self.moves.append(move)

    def update(self) -> None:
        pass
