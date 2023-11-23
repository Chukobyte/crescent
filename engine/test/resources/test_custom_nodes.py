import crescent_internal
from crescent import Node, NodeType


class TestNode(Node):
    def _start(self) -> None:
        print(f"Start called on {self}")

    def _end(self) -> None:
        print(f"End called on {self}")

    @staticmethod
    def new() -> "TestNode":
        return crescent_internal.node_new("test_custom_nodes", "TestNode", NodeType.Node)
