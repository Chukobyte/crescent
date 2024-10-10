from typing import Optional

import crescent_internal
from crescent import Node, SceneTree, Node2D, Vector2, GameProperties, Size2D, Camera2D, Rect2, World

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


def test_node_event_manager_callback(*args) -> None:
    assert not args


def test_node_event_manager_callback_one_param_string(*args) -> None:
    assert len(args) == 1
    assert args[0] == "Testing"


def test_node_event_manager_callback_one_param_int(*args) -> None:
    assert len(args) == 1
    assert args[0] == 42


def test_node_event_manager_callback_one_param_float(*args) -> None:
    assert len(args) == 1
    assert are_floats_equal(args[0], 10.0)


def test_node_event_manager_callback_one_param_bool(*args) -> None:
    assert len(args) == 1
    assert args[0]


with TestCase("Node Tests") as test_case:
    # Testing subscriptions
    subscriber_node = Node2D.new()
    root_node = SceneTree.get_root()
    root_node.add_child(subscriber_node)
    root_node.subscribe_to_event("talk", subscriber_node, test_node_event_manager_callback)
    root_node.subscribe_to_event("talk_string", subscriber_node, test_node_event_manager_callback_one_param_string)
    root_node.subscribe_to_event("talk_int", subscriber_node, test_node_event_manager_callback_one_param_int)
    root_node.subscribe_to_event("talk_float", subscriber_node, test_node_event_manager_callback_one_param_float)
    root_node.subscribe_to_event("talk_bool", subscriber_node, test_node_event_manager_callback_one_param_bool)

    # Testing node events
    test_node = Node2D.new()
    has_entered_scene = False
    has_exited_scene = False

    def on_has_entered_scene(*args):
        global has_entered_scene
        has_entered_scene = True

    def on_has_exited_scene(*args):
        global has_exited_scene
        has_exited_scene = True

    test_node.scene_entered.subscribe(root_node, on_has_entered_scene)
    test_node.scene_exited.subscribe(root_node, on_has_exited_scene)
    root_node.add_child(test_node)

    crescent_internal._scene_manager_process_queued_creation_entities()

    test_node.queue_deletion()

    assert has_entered_scene
    assert has_exited_scene

with TestCase("Scene Tree Tests") as test_case:
    SceneTree.change_scene("test_scene1.cscn")
    # TODO: Fix once scene manager updates are in
    # assert SceneTree.get_root()

with TestCase("Game Properties Tests") as test_case:
    game_props = GameProperties()
    assert game_props.game_title == "Test Game"
    assert game_props.game_resolution == Size2D(400, 300)
    assert game_props.default_window_size == Size2D(800, 600)
    assert game_props.target_fps == 66
    assert game_props.initial_scene_path == "main.cscn"
    assert not game_props.are_colliders_visible

with TestCase("Camera2D Tests") as test_case:
    Camera2D.set_position(Vector2(10, 20))
    assert Camera2D.get_position() == Vector2(10, 20)
    Camera2D.set_offset(Vector2(-5, -2))
    assert Camera2D.get_offset() == Vector2(-5, -2)
    Camera2D.set_zoom(Vector2(300, 400))
    assert Camera2D.get_zoom() == Vector2(300, 400)
    Camera2D.set_boundary(Rect2(10, 20, 300, 400))
    assert Camera2D.get_boundary() == Rect2(10, 20, 300, 400)
    # Test boundary
    Camera2D.set_position(Vector2(0, 0))
    assert Camera2D.get_position() == Vector2(10, 20)
    Camera2D.set_position(Vector2(2000, 2000))
    # Instead of testing for specific boundary position (that factors in screen resolution) we are just going to check to make sure it's not the value
    assert Camera2D.get_position().x < 2000
    assert Camera2D.get_position().y < 2000

with TestCase("World Tests") as test_case:
    World.set_time_dilation(2.0)
    assert World.get_time_dilation() == 2.0
    World.set_time_dilation(1.0)
    assert World.get_time_dilation() == 1.0
    assert World.get_delta_time() > 0.0
