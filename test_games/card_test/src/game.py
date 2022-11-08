import random

from crescent_api import *
from src.game_state import GameState
from src.mouse_tracker import MouseTracker
from src.card import Card, CardData


class Deck:
    def __init__(self):
        self.cards = []  # Contains card ids

    def add_card(self, card_id: str) -> None:
        self.cards.append(card_id)

    def draw(self, count: int) -> list:
        drawn_cards = []
        drawn_card_count = 0
        while drawn_card_count < count and self.cards:
            drawn_cards.append(self.cards.pop(0))
            drawn_card_count += 1
        return drawn_cards

    def shuffle(self) -> None:
        random.shuffle(self.cards)


class Game(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.mouse_tracker = MouseTracker()

    def _start(self) -> None:
        game_state = GameState()
        game_state.initialize(self)
        # Card test
        player_one_deck = Deck()
        player_one_deck.add_card(CardData.ID.CardOne)
        player_one_deck.add_card(CardData.ID.CardOne)
        cards_drawn = player_one_deck.draw(2)
        for index, card_id in enumerate(cards_drawn):
            card = Card.generate(cards_drawn[0])
            card.position = Vector2(200 + (index * 80), 440)
            card.z_index = 1
            card.location = Card.Location.Hand
            self.add_child(card)
            print(f"card_test = {card}")

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("ui_back"):
            Engine.exit()

        # All gameplay logic is centralized in mouse tracker for now...
        if Input.is_action_pressed("lmb"):
            self.mouse_tracker.process_left_mouse_pressed()

        if Input.is_action_just_released("lmb"):
            self.mouse_tracker.process_left_mouse_released()

        if Input.is_action_pressed("rmb"):
            print(
                f"mouse_position = {Input.Mouse.get_position()}, mouse_world_position = {Input.Mouse.get_world_position()}"
            )
