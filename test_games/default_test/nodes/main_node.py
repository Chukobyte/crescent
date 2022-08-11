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
                    class_path="src.main",
                    class_name="Main",
                ),
            ],
            children=[
                StageNode(
                    name="TestTextLabel",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(120.0, 200.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-32",
                            text="Default Project, Press 'ESC' to exit!",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
            ],
        )
    ]
)
