from crescent_api import *


class Main(Node2D):
    def _start(self) -> None:
        self.curve = CurveFloat(file_path="test.ccrv")
        self.time = 0.0
        print(f"curve.is_valid = {self.curve.valid}")

    # Simple curve test
    def _physics_update(self, delta_time: float) -> None:
        self.time += delta_time
        if self.time < 1.0:
            val = self.curve.eval(t=self.time)
            print(f"time = {self.time}, curve val = {val}")
