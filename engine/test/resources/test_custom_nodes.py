import crescent_internal
from crescent import Node, NodeType

start_call_count = 0
end_call_count = 0


def run_end_of_test_asserts() -> bool:
    assert start_call_count == 1
    assert end_call_count == 1
    return True


class TestNode(Node):
    def _start(self) -> None:
        global start_call_count
        start_call_count += 1

    def _end(self) -> None:
        global end_call_count
        end_call_count += 1

    @staticmethod
    def new() -> "TestNode":
        return crescent_internal.node_new("test_custom_nodes", "TestNode", NodeType.Node)
