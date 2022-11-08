from src.card_zone import *
from src.card import *


class TurnPhase:
    Init = "init"  # Only when game begins, goes straight into standby
    Standby = "standby"  # A pause before drawing begins, will probably add stuff later
    PlayerOneDraw = "p1_draw"
    PlayerOneMain = "p1_main"
    # TODO: Add p2 or comp stuff
    End = "end"


class GameState:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = object.__new__(cls)
            cls.turn_phase = TurnPhase.Init
            cls.card_zones = {}
            # cls.cards = {}
            cls.can_end_turn = False
            cls.turn_count = 1
            cls.max_turn_count = 6
            cls.turn_count_label = None  # TODO: Temp label node reference until stuff is moved out of game state
        return cls._instance

    def initialize(self, level_node: Node2D) -> None:
        self.reset()
        # Load card zone data
        for zone_name in [CardZone.Type.One, CardZone.Type.Two, CardZone.Type.Three]:
            self.card_zones[zone_name] = CardZone(level_node.get_child(zone_name))
        # Load temp card data
        # for card_name in [CardData.ID.CardOne]:
        #     self.cards[card_name] = CardData(level_node.get_child(card_name))
        # Set turn count label
        self.turn_count_label = level_node.get_child("TurnCountLabel")
        self.turn_count_label.set_text(f"Turn: {self.turn_count}")

    def reset(self) -> None:
        self.card_zones.clear()
        self.can_end_turn = False
        self.turn_count = 1
        self.turn_phase = TurnPhase.Init

    def get_card_zone_data(self, zone_name: str) -> CardZone:
        return self.card_zones.get(zone_name, None)

    # def get_card_data(self, card_name: str) -> CardData:
    #     return self.cards.get(card_name, None)

    def end_turn(self) -> bool:
        if self.can_end_turn:
            self.turn_count += 1
            self.turn_count_label.set_text(f"Turn: {self.turn_count}")
            self.can_end_turn = False
            self.turn_phase = TurnPhase.End
            return True
        return False
