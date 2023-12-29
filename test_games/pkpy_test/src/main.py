from crescent import Node2D, Input, Engine, TextLabel, Vector2, Particles2D


class Main(Node2D):
    def _start(self) -> None:
        text_label = TextLabel.new()
        text_label.text = "Hey"
        text_label.position = Vector2(30, 30)
        self.add_child(text_label)

        particles2d = Particles2D.new()
        particles2d.position = Vector2(300, 300)
        self.add_child(particles2d)

    def _process(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("exit"):
            Engine.exit()

    def _fixed_process(self, delta_time: float) -> None:
        pass
