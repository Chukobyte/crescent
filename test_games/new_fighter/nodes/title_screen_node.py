from crescent_api import *

create_stage_nodes(
    stage_nodes=[
        StageNode(
            name="TitleScreen",
            type="Node2D",
            tags=None,
            external_node_source=None,
            components=[
                Transform2DComponent(
                    position=Vector2(0, 0),
                    scale=Vector2(1, 1),
                    rotation=0,
                    z_index=0,
                    z_index_relative_to_parent=True,
                    ignore_camera=False,
                ),
                ScriptComponent(
                    class_path="src.title_screen",
                    class_name="TitleScreen",
                ),
            ],
            children=[],
        ),
    ],
)
