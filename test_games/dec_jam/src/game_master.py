import random
from typing import Tuple

from crescent_api import *
from src.enemy import GingerBreadMan, Elf, SnowMan
from src.utils.game_math import map_to_unit_range

game_properties = GameProperties()

# Setting boundary to the right of the player, the player will need to move left.
# Will update to reverse it once more levels are added.
LEVEL_BOUNDARY = Rect2(
    -432.0,
    -game_properties.game_resolution.h / 2.0,
    game_properties.game_resolution.w / 2.0,
    game_properties.game_resolution.h / 2.0,
)


# A class to hold game metadata for now...
class GameMetadata:
    def __init__(self, floor_y=0.0):
        self.floor_y = floor_y


class Level:
    def __init__(self):
        self.player_start_position = Vector2.ZERO()


class LevelCompletionItem(ColorRect):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.collider = None

    def _start(self) -> None:
        self.collider = Collider2D.new()
        self.collider.extents = self.size
        self.add_child(self.collider)


class GameMaster:
    def __init__(self, player: Node2D):
        self.player = player
        self.metadata = GameMetadata(
            floor_y=player.position.y
        )  # TODO: Replace with real value...
        self.time_since_last_enemy_spawn = 0.0
        self.enemies_active = 0
        self.enemies_active_limit = 4
        self.current_level = Level()
        self.level_completion_item: LevelCompletionItem = LevelCompletionItem.new()

    def initialize_level(self) -> None:
        self.level_completion_item.size = Size2D(4, 4)
        self.level_completion_item.color = Color.linear_color(0.8, 0.1, 0.8)
        self.level_completion_item.position = Vector2(
            LEVEL_BOUNDARY.x, self.player.position.y
        )
        SceneTree.get_root().add_child(self.level_completion_item)
        self.current_level.player_start_position = self.player.position

    def update(self, delta_time: float) -> None:
        # print(f"dist from end normalized = '{self._get_distance_to_end_value_normalized()}'")
        self.time_since_last_enemy_spawn += delta_time
        if (
            self.time_since_last_enemy_spawn >= 2.0
            and self.enemies_active < self.enemies_active_limit
        ):
            self.spawn_random_enemy()
            self.time_since_last_enemy_spawn = 0.0

    def spawn_random_enemy(self) -> None:
        pos_offset = Vector2(128.0, -8.0)
        rand_n = random.choice([0, 1, 2])
        if rand_n == 0:
            enemy_node = GingerBreadMan.new()
        elif rand_n == 1:
            enemy_node = Elf.new()
            pos_offset += Vector2(0, 11)
        else:
            enemy_node = SnowMan.new()

        new_pos, dir_vector = self._get_valid_enemy_spawn_pos_and_move_dir(pos_offset)
        enemy_node.position = new_pos
        # Invert the x to get direction facing
        enemy_node.direction_facing = dir_vector
        enemy_node.subscribe_to_event(
            event_id="scene_exited",
            scoped_node=self.player,
            callback_func=lambda node: self._set_enemies_active(
                value=self.enemies_active - 1,
            ),
        )
        SceneTree.get_root().add_child(enemy_node)
        self.enemies_active += 1

    def _get_valid_enemy_spawn_pos_and_move_dir(
        self, pos_offset=Vector2.ZERO()
    ) -> Tuple[Vector2, Vector2]:
        left_pos = Vector2(self.player.position.x, self.metadata.floor_y) + Vector2(
            pos_offset.x * -1.0, pos_offset.y
        )
        right_pos = Vector2(self.player.position.x, self.metadata.floor_y) + Vector2(
            pos_offset.x * 1.0, pos_offset.y
        )
        positions = []
        if LEVEL_BOUNDARY.contains_point(left_pos):
            positions.append(left_pos)
        if LEVEL_BOUNDARY.contains_point(right_pos):
            positions.append(right_pos)
        if positions:
            new_pos = random.choice(positions)
            if new_pos == right_pos:
                move_dir = Vector2.LEFT()
            else:
                move_dir = Vector2.RIGHT()
            return new_pos, move_dir
        print("***ERROR: didn't find a valid spawn position, returning the left pos!")
        return left_pos, Vector2.RIGHT()

    def _set_enemies_active(self, value: int) -> None:
        self.enemies_active = value

    def _get_distance_to_end_value_normalized(self) -> float:
        beginning_to_end_dist = self.current_level.player_start_position.distance_to(
            self.level_completion_item.position
        )
        player_to_end_dist = self.player.position.distance_to(
            self.level_completion_item.position
        )
        player_dist_to_end_range = map_to_unit_range(
            player_to_end_dist, beginning_to_end_dist, 0.0
        )
        return player_dist_to_end_range
