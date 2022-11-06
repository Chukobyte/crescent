from src.card_zone import *
from src.card import *


class GameState:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = object.__new__(cls)
            cls.card_zones = {}
            cls.cards = {}
        return cls._instance

    def initialize(self, level_node: Node2D) -> None:
        self.reset()
        # Load card zone data
        for zone_name in [CardZone.Type.One, CardZone.Type.Two, CardZone.Type.Three]:
            self.card_zones[zone_name] = CardZone(level_node.get_child(zone_name))
        # Load temp card data
        for card_name in [Card.Name.CardOne]:
            self.cards[card_name] = Card(level_node.get_child(card_name))

    def reset(self) -> None:
        self.card_zones.clear()

    def get_card_zone_data(self, zone_name: str) -> CardZone:
        return self.card_zones.get(zone_name, None)

    def get_card_data(self, card_name: str) -> Card:
        return self.cards.get(card_name, None)
