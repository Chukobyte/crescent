from crescent_api import *
from src.card import Card


class CardZone:
    class Type:
        One = "CardZone0"
        Two = "CardZone1"
        Three = "CardZone2"

    def __init__(self, node: ColorRect):
        self.node = node
        self.player_one_score_label = node.get_child("PlayerOnePoints")
        self._player_one_score = 0
        self.player_one_card_slots = []
        self._player_two_score = 0
        self.player_two_score_label = node.get_child("PlayerTwoPoints")
        self.player_two_card_slots = []

    @property
    def player_one_score(self) -> int:
        return self._player_one_score

    @player_one_score.setter
    def player_one_score(self, value: int) -> None:
        self._player_one_score = value
        self.player_one_score_label.set_text(str(self._player_one_score))

    @property
    def player_two_score(self) -> int:
        return int(self.player_two_score_label.get_text())

    @player_two_score.setter
    def player_two_score(self, value: int) -> None:
        self.player_two_score_label.set_text(str(value))

    def add_card_to_player_one_slot(self, card: Card) -> None:
        self.player_one_score += card.power
        card.location = Card.Location.Zone
        self.player_one_card_slots.append(card)
        self._adjust_player_one_card_positions()

    def remove_card_from_player_one_slot(self, card: Card) -> None:
        if card in self.player_one_card_slots:
            self.player_one_score -= card.power
            self.player_one_card_slots.remove(card)
            self._adjust_player_one_card_positions()

    def _adjust_player_one_card_positions(self) -> None:
        card_zone_position = self.node.position
        cards_per_row = 2
        current_cards_in_row = 0
        row_index = 0
        y_pos = 160
        for index, card in enumerate(self.player_one_card_slots):
            if current_cards_in_row >= cards_per_row:
                current_cards_in_row = 0
                row_index = 0
                y_pos += 70
            card.position = card_zone_position + Vector2(0 + (row_index * 64), y_pos)
            current_cards_in_row += 1
            row_index += 1
