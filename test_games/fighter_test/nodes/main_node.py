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
                    position=Vector2(175.0, 71.0),
                    scale=Vector2(4.0, 4.0),
                    rotation=0.0,
                    z_index=0,
                    z_index_relative_to_parent=True,
                    ignore_camera=False,
                ),
                SpriteComponent(
                    texture_path="assets/images/melissa_walk_animation.png",
                    draw_source=Rect2(0.0, 0.0, 48.0, 48.0),
                    flip_x=False,
                    flip_y=False,
                    modulate=Color.normalize(255, 255, 255, 255)
                ),
                ScriptComponent(
                    class_path="src.main",
                    class_name="Main"
                ),
            ],
            children=[]
        )
    ]
)
