from crescent_api import *

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
                    type="AnimatedSprite",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(200.0, 368.0),
                            scale=Vector2(4.0, 4.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        AnimatedSpriteComponent(
                            current_animation_name="idle",
                            is_playing=True,
                            origin=Vector2(24, 27),
                            animations=[
                                Animation(
                                    name="idle",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=8,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=496, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=9,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=558, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=10,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=620, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="walk-forward",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=8,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=496, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=9,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=558, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=10,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=620, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=11,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=682, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=12,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=744, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="walk-backward",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=8,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=496, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=9,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=558, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="crouch",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="jump",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_jump_up_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                            ],
                        ),
                    ],
                    children=[
                        StageNode(
                            name="Collider",
                            type="Collider2D",
                            tags=None,
                            external_node_source=None,
                            components=[
                                Transform2DComponent(
                                    position=Vector2(-8.0, -7.0),
                                    scale=Vector2(1.0, 1.0),
                                    rotation=0.0,
                                    z_index=0,
                                    z_index_relative_to_parent=True,
                                    ignore_camera=False,
                                ),
                                Collider2DComponent(
                                    extents=Size2D(16, 17),
                                    color=Color(200, 200, 255, 200),
                                ),
                            ],
                            children=[],
                        ),
                    ],
                ),
                StageNode(
                    name="PlayerTwo",
                    type="AnimatedSprite",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(550.0, 368.0),
                            scale=Vector2(-4.0, 4.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        AnimatedSpriteComponent(
                            current_animation_name="idle",
                            is_playing=True,
                            origin=Vector2(24, 27),
                            flip_x=False,
                            animations=[
                                Animation(
                                    name="idle",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=8,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=496, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=9,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=558, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=10,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_idle_sheet.png",
                                            draw_source=Rect2(x=620, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="walk-forward",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=8,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=496, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=9,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=558, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=10,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=620, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=11,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=682, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=12,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_forward_sheet.png",
                                            draw_source=Rect2(x=744, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="walk-backward",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=8,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=496, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=9,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_walk_backward_sheet.png",
                                            draw_source=Rect2(x=558, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="crouch",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=1,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=62, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=2,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=124, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=3,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=186, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=4,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=248, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=5,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=310, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=6,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=372, y=0, w=62, h=37),
                                        ),
                                        AnimationFrame(
                                            frame=7,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_crouch_idle_sheet.png",
                                            draw_source=Rect2(x=434, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                                Animation(
                                    name="jump",
                                    speed=100,
                                    loops=True,
                                    frames=[
                                        AnimationFrame(
                                            frame=0,
                                            texture_path="test_games/fighter_test/assets/images/characters/mor/mor_jump_up_sheet.png",
                                            draw_source=Rect2(x=0, y=0, w=62, h=37),
                                        ),
                                    ],
                                ),
                            ],
                        ),
                    ],
                    children=[
                        StageNode(
                            name="Collider",
                            type="Collider2D",
                            tags=None,
                            external_node_source=None,
                            components=[
                                Transform2DComponent(
                                    position=Vector2(-8.0, -7.0),
                                    scale=Vector2(1.0, 1.0),
                                    rotation=0.0,
                                    z_index=0,
                                    z_index_relative_to_parent=True,
                                    ignore_camera=False,
                                ),
                                Collider2DComponent(
                                    extents=Size2D(16, 17),
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
                            ignore_camera=True,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Fight Game",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="TimeDisplay",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(380.0, 50.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=True,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="60",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="P1HealthBar",
                    type="ColorSquare",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(100.0, 80.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=True,
                        ),
                        ColorSquareComponent(
                            size=Size2D(100, 20),
                            color=Color(255, 0, 0),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="P2HealthBar",
                    type="ColorSquare",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(600.0, 80.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=True,
                        ),
                        ColorSquareComponent(
                            size=Size2D(100, 20),
                            color=Color(255, 0, 0),
                        ),
                    ],
                    children=[],
                ),
            ],
        )
    ]
)
