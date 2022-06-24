from rbe_py_api import *

create_stage_nodes(
    stage_nodes=[
        StageNode(
            name="Main",
            type="Node2D",
            tags=None,
            external_node_source=None,
            components=[
                Transform2DComponent(
                    position=Vector2(0.0, 0.0),
                    scale=Vector2(1.0, 1.0),
                    rotation=0.0,
                    z_index=0,
                    z_index_relative_to_parent=True,
                    ignore_camera=False,
                ),
                ScriptComponent(
                    class_path="test_games.fighter_test.src.main",
                    class_name="Main",
                ),
            ],
            children=[
                StageNode(
                    name="PlayerOne",
                    type="Sprite",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(250.0, 300.0),
                            scale=Vector2(2.0, 2.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        AnimatedSpriteComponent(
                            current_animation_name="walk",
                            is_playing=True,
                            animations=[
                                Animation(
                                    name="walk",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=0, y=0, w=32, h=32),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=32, y=0, w=32, h=32),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=64, y=0, w=32, h=32),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=96, y=0, w=32, h=32),
                                        ),
                                    ],
                                )
                            ],
                        ),
                    ],
                    children=[
                        StageNode(
                            name="P1Collider",
                            type="Collider2D",
                            tags=None,
                            external_node_source=None,
                            components=[
                                Transform2DComponent(
                                    position=Vector2(0.0, 0.0),
                                    scale=Vector2(1.0, 1.0),
                                    rotation=0.0,
                                    z_index=0,
                                    z_index_relative_to_parent=True,
                                    ignore_camera=False,
                                ),
                                Collider2DComponent(
                                    rect=Rect2(0, 0, 32, 32),
                                    color=Color(200, 200, 255, 200),
                                ),
                            ],
                            children=[],
                        ),
                    ],
                ),
                StageNode(
                    name="PlayerTwo",
                    type="Sprite",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(450.0, 300.0),
                            scale=Vector2(2.0, 2.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        AnimatedSpriteComponent(
                            current_animation_name="walk",
                            is_playing=True,
                            flip_x=True,
                            animations=[
                                Animation(
                                    name="walk",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=0, y=0, w=32, h=32),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=32, y=0, w=32, h=32),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=64, y=0, w=32, h=32),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/melissa_walk_animation.png",
                                            draw_source=Rect2(x=96, y=0, w=32, h=32),
                                        ),
                                    ],
                                )
                            ],
                        ),
                    ],
                    children=[
                        StageNode(
                            name="P2Collider",
                            type="Collider2D",
                            tags=None,
                            external_node_source=None,
                            components=[
                                Transform2DComponent(
                                    position=Vector2(0.0, 0.0),
                                    scale=Vector2(1.0, 1.0),
                                    rotation=0.0,
                                    z_index=0,
                                    z_index_relative_to_parent=True,
                                    ignore_camera=False,
                                ),
                                Collider2DComponent(
                                    rect=Rect2(0, 0, 32, 32),
                                    color=Color(200, 200, 255, 200),
                                ),
                            ],
                            children=[],
                        ),
                    ],
                ),
                StageNode(
                    name="TextLabel",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(250.0, 200.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Fight Game",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
            ],
        )
    ]
)
