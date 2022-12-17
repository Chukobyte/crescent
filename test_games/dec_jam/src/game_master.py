import random

from crescent_api import Node2D, Vector2, SceneTree
from src.enemy import GingerBreadMan

# TODO: Need to add utilities to python api to query the scene tree for things (maybe by tags)


# A class to hold game metadata for now...
class GameMetadata:
    def __init__(self, floor_y=0.0):
        self.floor_y = floor_y


class GameMaster:
    def __init__(self, player: Node2D):
        self.player = player
        self.metadata = GameMetadata(
            floor_y=player.position.y
        )  # TODO: Replace with real value...
        self.time_since_last_enemy_spawn = 0.0
        self.enemies_active = 0

    def update(self, delta_time: float) -> None:
        self.time_since_last_enemy_spawn += delta_time
        if self.time_since_last_enemy_spawn >= 2.0 and self.enemies_active < 4:
            self.spawn_enemy()
            self.time_since_last_enemy_spawn = 0.0

    def spawn_enemy(self) -> None:
        # Determine which side the enemy spawns
        if random.choice([0, 1]) == 0:
            dir_vector = Vector2.RIGHT()
        else:
            dir_vector = Vector2.LEFT()
        # Determine the y position of the enemy
        rand_num = random.choice([0, 2])
        if rand_num == 0:
            y_pos = 32
        elif rand_num == 1:
            y_pos = -64
        else:
            y_pos = -96
        pos_offset = Vector2(0, y_pos)

        ginger_bread_man = GingerBreadMan.new()
        ginger_bread_man.position = (
            Vector2(self.player.position.x, self.metadata.floor_y)
            + (Vector2(128, y_pos) * dir_vector)
            + pos_offset
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
