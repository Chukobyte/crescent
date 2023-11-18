import crescent


class TestCase:
    def __init__(self, name: str) -> None:
        self.name = name

    def __enter__(self):
        self._test_header()

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._test_footer()

    def _test_header(self):
        print(f"--------------- {self.name} ---------------")

    def _test_footer(self):
        print("---------------------------------------")

# Tests
with TestCase("Node New"):
    pass

