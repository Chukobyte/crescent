from crescent_api import Node2D, Vector2
from src.enemy import GingerBreadMan

# TODO: Need to add utilities to python api to query the scene tree for things (maybe by tags)


class GameMaster:
    def __init__(self, player: Node2D):
        self.player = player
        self.time_since_last_enemy_spawn = 0.0
        self.enemies_active = 0

    def update(self, delta_time: float) -> None:
        self.time_since_last_enemy_spawn += delta_time
        if self.time_since_last_enemy_spawn >= 2.0 and self.enemies_active == 0:
            self.spawn_enemy()

    def spawn_enemy(self) -> None:
        ginger_bread_man = GingerBreadMan.new()
        ginger_bread_man.position = self.player.position + Vector2(64, 0)
        self.player.get_parent().add_child(ginger_bread_man)
        self.enemies_active += 1
        print("spawn_enemy")


# TODO: Make a singleton when levels are added
# _instance = None
#
# def __new__(cls, *args, **kwargs):
#     if not cls._instance:
#         cls._instance = object.__new__(cls)
#     return cls._instance
