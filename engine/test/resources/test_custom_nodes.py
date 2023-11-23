import crescent_internal
from crescent import Node, NodeType


class TestNode(Node):
    def _start(self) -> None:
        print(f"Start called on {self}")

    @staticmethod
    def new() -> "TestNode":
        return crescent_internal.node_new("crescent_api_test", "TestNode", NodeType.Node)
