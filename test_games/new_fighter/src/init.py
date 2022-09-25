from crescent_api import *


class Init(Node2D):
    def _start(self) -> None:
        SceneTree.change_scene(path="nodes/title_screen_node.cscn")
