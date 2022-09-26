class Timer:
    def __init__(self, time: float):
        self.time = time
        self.time_remaining = 0.0

    def tick(self, delta_time: float) -> float:
        self.time -= delta_time
        return max(self.time, 0.0)
