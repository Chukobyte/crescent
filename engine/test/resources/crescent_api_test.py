from typing import Optional

import crescent_internal
from crescent import Node, SceneTree, Node2D, Vector2

# import nonexistant  # TODO: uncomment and test custom import handler
import test_custom_nodes


class TestCase:
    def __init__(self, name: str) -> None:
        self.name = name

    def __enter__(self) -> "TestCase":
        self._test_header()
        return self

    def __exit__(self, *args):
        self._test_footer()

    def _test_header(self):
        print(f"--------------- {self.name} ---------------")

    def _test_footer(self):
        print("---------------------------------------")


def are_floats_equal(a: float, b: float, rel_tol=0.00000001, abs_tol=0.0) -> bool:
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)


# Tests
with TestCase("Node Tests") as test_case:
    # Create node
    new_node = Node.new()
    assert new_node
    scene_root = SceneTree.get_root()
    assert scene_root
    scene_root.add_child(new_node)
    # Node name
    node_name = new_node.name
    assert node_name
    # Add Child
    assert len(new_node.get_children()) == 0
    new_node_child1 = Node.new()
    new_node.add_child(new_node_child1)
    # Get Child
    assert len(new_node.get_children()) == 1
    # Time Dilation
    new_node.set_time_dilation(2.0)
    assert are_floats_equal(new_node.get_time_dilation(), 2.0)
    new_node.time_dilation = 1.0
    assert are_floats_equal(new_node.time_dilation, 1.0)
    # Test Custom Node
    test_node = test_custom_nodes.TestNode.new()
    new_node.add_child(test_node)

    crescent_internal._scene_manager_process_queued_creation_entities()

    new_node.queue_deletion()
    assert new_node.is_queued_for_deletion()

    assert test_custom_nodes.run_end_of_test_asserts()

with TestCase("Node2D Tests") as test_case:
    node2d = Node2D.new()
    assert node2d
    scene_root = SceneTree.get_root()
    assert scene_root
    scene_root.add_child(node2d)
    # Test position functions
    node2d.set_position(Vector2(10, 15))
    assert node2d.get_position() == Vector2(10, 15)
    node2d.add_to_position(Vector2(3, 10))
    assert node2d.get_position() == Vector2(13, 25)
    assert node2d.position == Vector2(13, 25)
    node2d.position = Vector2(1, 3)
    assert node2d.position == Vector2(1, 3)
    # Test scale functions
    node2d.set_scale(Vector2(3, 5))
    assert node2d.get_scale() == Vector2(3, 5)
    node2d.add_to_scale(Vector2(2, 30))
    assert node2d.get_scale() == Vector2(5, 35)
    node2d.scale = Vector2(2, 3)
    assert node2d.scale == Vector2(2, 3)
    # Test rotation functions
    node2d.set_rotation(20.0)
    assert node2d.get_rotation() == 20.0
    node2d.add_to_rotation(5.0)
    assert node2d.get_rotation() == 25.0
    node2d.rotation = 180.0
    assert node2d.rotation == 180.0
    # Test z index functions
    node2d.z_index = 10
    assert node2d.z_index == 10
    node2d.z_index = 0
    assert node2d.z_index == 0
    node2d.z_index_relative_to_parent = False
    assert not node2d.z_index_relative_to_parent
    node2d.z_index_relative_to_parent = True
    assert node2d.z_index_relative_to_parent
    # Test ignore camera functions
    node2d.ignore_camera = False
    assert not node2d.ignore_camera
    node2d.ignore_camera = True
    assert node2d.ignore_camera

    crescent_internal._scene_manager_process_queued_creation_entities()

    node2d.queue_deletion()


with TestCase("Scene Tree Tests") as test_case:
    SceneTree.change_scene("test_scene1.cscn")
    # TODO: Fix once scene manager updates are in
    # assert SceneTree.get_root()
