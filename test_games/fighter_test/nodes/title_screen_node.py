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
                    position=Vector2(0.0, 0.0),
                    scale=Vector2(1.0, 1.0),
                    rotation=0.0,
                    z_index=0,
                    z_index_relative_to_parent=True,
                    ignore_camera=False,
                ),
                ScriptComponent(
                    class_path="src.title_screen",
                    class_name="TitleScreen",
                ),
            ],
            children=[
                StageNode(
                    name="InstructionsText",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(130.0, 200.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Press Enter to Play!",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="ModeText",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(80.0, 300.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Mode:",
                            color=Color(255, 255, 255),
                        ),
                    ],
                    children=[],
                ),
                StageNode(
                    name="ConnectText",
                    type="TextLabel",
                    tags=None,
                    external_node_source=None,
                    components=[
                        Transform2DComponent(
                            position=Vector2(60.0, 390.0),
                            scale=Vector2(1.0, 1.0),
                            rotation=0.0,
                            z_index=0,
                            z_index_relative_to_parent=True,
                            ignore_camera=False,
                        ),
                        TextLabelComponent(
                            uid="fight-64",
                            text="Waiting for connection...",
                            color=Color(255, 255, 255, 0),
                        ),
                    ],
                    children=[],
                ),
            ],
        )
    ]
)
