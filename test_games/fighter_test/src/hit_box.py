from rbe_py_api import *


class HitBox(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.listener_funcs = []  # Takes (collider, collided_entities) as params

    def register_listener_func(self, func) -> None:
        self.listener_funcs.append(func)

    # Temp
    # def _physics_update(self, delta_time: float) -> None:
    #     collided_entities = CollisionHandler.process_collisions(self)
    #     for listener_func in self.listener_funcs:
    #         listener_func(self, collided_entities)


class Attack(HitBox):
    def start(self) -> None:
        self.set_rect(Rect2(0, 0, 32, 32))
