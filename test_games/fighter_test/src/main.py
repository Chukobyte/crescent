from rbe_py_api import *


class Main(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        print(f"[SCRIPT] Entity entered stage with id = {self.entity_id}")

    def _start(self) -> None:
        pass

    def _update(self, delta_time: float) -> None:
        pass

    def _end(self) -> None:
        pass
