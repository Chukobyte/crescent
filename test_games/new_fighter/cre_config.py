from crescent_api import *

configure_game(
   game_title="New Fighter",
   window_width=800,
   window_height=600,
   resolution_width=800,
   resolution_height=600,
   target_fps=66,
   initial_node_path="nodes/init_node.py",
   colliders_visible=False,
)

configure_assets(
   audio_sources=[
   ],
   textures=[
   ],
   fonts=[
   ],
)

configure_inputs(
   input_actions=[
       InputAction(name="quit", values=[Input.Keyboard.ESC]),
   ],
)
