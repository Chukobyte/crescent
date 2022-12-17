import random

from crescent_api import Node2D, Vector2, SceneTree
from src.enemy import GingerBreadMan

# TODO: Need to add utilities to python api to query the scene tree for things (maybe by tags)


class GameMaster:
    def __init__(self, player: Node2D):
        self.player = player
        self.time_since_last_enemy_spawn = 0.0
        self.enemies_active = 0

    def update(self, delta_time: float) -> None:
        self.time_since_last_enemy_spawn += delta_time
        if self.time_since_last_enemy_spawn >= 2.0 and self.enemies_active < 4:
            self.spawn_enemy()
            self.time_since_last_enemy_spawn = 0.0

    def spawn_enemy(self) -> None:
        if random.choice([0, 1]) == 0:
            dir_vector = Vector2.RIGHT()
        else:
            dir_vector = Vector2.LEFT()
        ginger_bread_man = GingerBreadMan.new()
        ginger_bread_man.position = self.player.position + (
            Vector2(128, 0) * dir_vector
        )
        ginger_bread_man.subscribe_to_event(
            event_id="scene_exited",
            scoped_node=self.player,
            callback_func=lambda node: self._set_enemies_active(
                value=self.enemies_active - 1,
            ),
        )
        SceneTree.get_root().add_child(ginger_bread_man)
        self.enemies_active += 1

    def _set_enemies_active(self, value: int) -> None:
        self.enemies_active = value


# TODO: Make a singleton when levels are added
# _instance = None
#
# def __new__(cls, *args, **kwargs):
#     if not cls._instance:
#         cls._instance = object.__new__(cls)
#     return cls._instance
