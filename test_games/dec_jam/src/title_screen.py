import math
import random
from typing import List

from crescent_api import (
    Node2D,
    Input,
    Engine,
    SceneTree,
    ColorRect,
    Vector2,
    Color,
    Size2D,
    GameProperties,
    ShaderUtil,
)

SPEED = 5


class Snow(ColorRect):
    def _physics_update(self, delta_time: float) -> None:
        global SPEED
        pos = self.position
        props = GameProperties()
        grace_padding = 16
        if (
            pos.x < 0.0 - grace_padding
            # or pos.x > props.game_resolution.w + grace_padding
            # or pos.y < 0.0 - grace_padding
            or pos.y > props.game_resolution.h + grace_padding
        ):
            self.broadcast_event(event_id="offscreen", args=self)
        self.add_to_position(Vector2((-SPEED * 2.0) * delta_time, SPEED * delta_time))


class SnowManager:
    def __init__(self):
        self.snow_pool = []

    def create_snow(self, count: int) -> None:
        size = Size2D(2, 2)
        props = GameProperties()
        for i in range(count):
            snow_node = Snow.new()
            snow_node.ignore_camera = True
            # pos_x = (i * (size.w + 2)) % props.game_resolution.w
            # pos_y = math.floor((i / 10)) * (size.h + 2)
            # snow_node.position = Vector2(pos_x, pos_y)
            snow_node.position = Vector2(-10000, -10000)
            snow_node.color = Color.linear_color(0.8, 0.8, 0.8)
            snow_node.size = size
            # snow_node.subscribe_to_event(
            #     event_id="scene_exited",
            #     scoped_node=SceneTree.get_root(),
            #     callback_func=lambda node: self.snow_pool.remove(node),
            # )
            snow_node.subscribe_to_event(
                event_id="offscreen",
                scoped_node=SceneTree.get_root(),
                callback_func=lambda node: self.add_and_deque_snow_temp(node),
            )
            SceneTree.get_root().add_child(snow_node)
            self.add_snow_to_queue(snow_node)

        self.deque_snow(count)

    def deque_snow(self, count: int) -> None:
        props = GameProperties()
        for i in range(count):
            if not self.snow_pool:
                break
            snow = self.snow_pool.pop()
            new_pos = Vector2(
                props.game_resolution.w + random.uniform(0.0, 128.0),
                random.uniform(-8.0, -32.0),
            )
            snow.position = new_pos

    def add_snow_to_queue(self, snow: Snow) -> None:
        self.snow_pool.append(snow)

    def add_and_deque_snow_temp(self, snow: Snow) -> None:
        self.add_snow_to_queue(snow=snow)
        self.deque_snow(count=1)


class TitleScreen(Node2D):
    def _start(self) -> None:
        # SnowManager().create_snow(50)
        ShaderUtil.compile_shader(
            shader_id="screen",
            vertex_path="shaders/screen.vsh",
            fragment_path="shaders/screen.fsh",
        )

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

        if Input.is_action_just_pressed(name="attack"):
            SceneTree.change_scene("scenes/main.cscn")
