import crescent_internal
from crescent import Node2D, NodeType

start_call_count = 0
process_call_count = 0
fixed_process_call_count = 0
end_call_count = 0


def run_end_of_test_asserts() -> bool:
    """
    Execute asserts for testing node callback functions
    :return:
    bool: Whether all asserts and tests have passed
    """
    assert start_call_count == 1
    assert end_call_count == 1
    # TODO: Test once exposed ec system calls for testing
    # assert process_call_count == 1
    # assert fixed_process_call_count == 1
    return True


# TODO: Node breaks on not calling '_start', '_end', etc... look into later or don't user Node...

class TestNode(Node2D):
    def _start(self) -> None:
        print("No")
        global start_call_count
        start_call_count += 1

    def _process(self, delta_time: float) -> None:
        global process_call_count
        process_call_count += 1

    def _fixed_process(self, delta_time: float) -> None:
        global fixed_process_call_count
        fixed_process_call_count += 1

    def _end(self) -> None:
        global end_call_count
        end_call_count += 1
