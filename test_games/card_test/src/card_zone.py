from crescent_api import *


class CardZone:
    class Type:
        One = "CardZone0"
        Two = "CardZone1"
        Three = "CardZone2"

    def __init__(self, node: ColorRect):
        self.node = node
        self.player_one_score_label = node.get_child("PlayerOnePoints")
        self.player_two_score_label = node.get_child("PlayerTwoPoints")

    @property
    def player_one_score(self) -> int:
        return int(self.player_one_score_label.get_text())

    @player_one_score.setter
    def player_one_score(self, value: int) -> None:
        self.player_one_score_label.set_text(str(value))

    @property
    def player_two_score(self) -> int:
        return int(self.player_two_score_label.get_text())

    @player_two_score.setter
    def player_two_score(self, value: int) -> None:
        self.player_two_score_label.set_text(str(value))
