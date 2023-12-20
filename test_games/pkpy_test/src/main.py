from crescent import Node2D, Input, Engine, TextLabel, Vector2


class Main(Node2D):
    def _start(self) -> None:
        text_label = TextLabel.new()
        text_label.text = "Hey"
        text_label.position = Vector2(30, 30)
        self.add_child(text_label)

    def _process(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("exit"):
            Engine.exit()

    def _fixed_process(self, delta_time: float) -> None:
        print(f"fixed_update: delta_time = {delta_time}")
