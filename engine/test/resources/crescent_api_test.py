from typing import Optional

from crescent import Node, SceneTree


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
    new_node = Node.new("crescent", "Node")
    assert new_node
    # Node name
    node_name = new_node.name
    assert node_name
    print(f"node_name = {node_name}")
    # Add Child
    # Get Child
    assert not new_node.get_children()
    # Time Dilation
    new_node.set_time_dilation(2.0)
    assert are_floats_equal(new_node.get_time_dilation(), 2.0)
    new_node.time_dilation = 1.0
    assert are_floats_equal(new_node.time_dilation, 1.0)
    # TODO: Queue delete once scene manager updates properly in tests
    new_node.queue_deletion()
    # assert new_node.is_queued_for_deletion()

with TestCase("Scene Tree Tests") as test_case:
    SceneTree.change_scene("test_scene1.cscn")
    # TODO: Fix once scene manager updates are in
    # assert SceneTree.get_root()
