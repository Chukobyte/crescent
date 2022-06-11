from rbe_py_api import *


class Main(Node2D):
    def _start(self) -> None:
        print(f"[SCRIPT] Entity entered stage with id = {self.entity_id}")

    def _update(self, delta_time: float) -> None:
        print("Hey")

    def _end(self) -> None:
        pass
