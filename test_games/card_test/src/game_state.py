from src.card_zone import *


class GameState:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = object.__new__(cls)
            cls.card_zones = {}
        return cls._instance

    def initialize(self, level_node: Node2D) -> None:
        self.reset()
        for zone_name in [CardZone.Type.One, CardZone.Type.Two, CardZone.Type.Three]:
            self.card_zones[zone_name] = CardZone(level_node.get_child(zone_name))

    def reset(self) -> None:
        self.card_zones.clear()

    def get_card_zone_data(self, zone_name: str) -> CardZone:
        return self.card_zones.get(zone_name, None)
