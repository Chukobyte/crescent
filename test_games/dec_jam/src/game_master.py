import random

from crescent_api import Node2D, Vector2, SceneTree, Rect2
from src.enemy import GingerBreadMan, Elf
from src.utils.math import clamp_pos_to_boundary


# TODO: Need to add utilities to python api to query the scene tree for things (maybe by tags)


# Need a place to store generic data about the game

GAME_RESOLUTION = Vector2(800, 450)
# Setting boundary to the right of the player, the player will need to move left.
# Will update to reverse it once more levels are added.
LEVEL_BOUNDARY = Rect2(
    -3200.0,
    -GAME_RESOLUTION.y / 2.0,
    GAME_RESOLUTION.x / 2.0,
    GAME_RESOLUTION.y / 2.0,
)


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

        pos_offset = Vector2(128.0, -16.0)
        # if random.choice([0, 1]) == 0: # TODO: Fix Elf collision breaking
        if random.choice([0]) == 0:
            enemy_node = GingerBreadMan.new()
        else:
            enemy_node = Elf.new()
            pos_offset += Vector2(0, 32)

        new_pos = Vector2(self.player.position.x, self.metadata.floor_y) + Vector2(
            pos_offset.x * dir_vector.x, pos_offset.y
        )
        new_pos = clamp_pos_to_boundary(new_pos, LEVEL_BOUNDARY)
        enemy_node.position = new_pos
        enemy_node.subscribe_to_event(
            event_id="scene_exited",
            scoped_node=self.player,
            callback_func=lambda node: self._set_enemies_active(
                value=self.enemies_active - 1,
            ),
        )
        SceneTree.get_root().add_child(enemy_node)
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
