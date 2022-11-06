from crescent_api import *


class Card:
    class Name:
        CardOne = "Card0"

    def __init__(self, card_node: ColorRect):
        self.node = card_node
        self.is_in_hand = True

    @property
    def name(self) -> str:
        return self.node.name
