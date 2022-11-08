import random

from crescent_api import *
from src.game_state import GameState, TurnPhase
from src.mouse_tracker import MouseTracker
from src.card import Card, CardData
from src.task import co_suspend, co_return, co_wait_seconds


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
        self.current_turn_task = self.turn_task()

    def _start(self) -> None:
        game_state = GameState()
        game_state.initialize(self)

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed("ui_back"):
            Engine.exit()

        # Call our main task
        awaitable = self.current_turn_task.send(None)

        # All gameplay logic is centralized in mouse tracker for now...
        if Input.is_action_pressed("lmb"):
            self.mouse_tracker.process_left_mouse_pressed()

        if Input.is_action_just_released("lmb"):
            self.mouse_tracker.process_left_mouse_released()

        if Input.is_action_pressed("rmb"):
            print(
                f"mouse_position = {Input.Mouse.get_position()}, mouse_world_position = {Input.Mouse.get_world_position()}"
            )

    async def turn_task(self):
        try:
            game_state = GameState()
            player_one_deck = Deck()
            for card_id in [CardData.ID.CardOne, CardData.ID.CardOne, CardData.ID.CardOne, CardData.ID.CardOne]:
                player_one_deck.add_card(card_id)
            while True:
                if game_state.turn_phase == TurnPhase.Init:
                    game_state.turn_phase = TurnPhase.Standby
                elif game_state.turn_phase == TurnPhase.Standby:
                    await co_wait_seconds(1.0)
                    game_state.turn_phase = TurnPhase.PlayerOneDraw
                elif game_state.turn_phase == TurnPhase.PlayerOneDraw:
                    cards_drawn = player_one_deck.draw(2)
                    for index, card_id in enumerate(cards_drawn):
                        card = Card.generate(card_id)
                        card.position = Vector2(200 + (index * 80), 440)  # TODO: Implement slot positions for hand
                        card.z_index = 1
                        card.location = Card.Location.Hand
                        self.add_child(card)
                        await co_wait_seconds(0.2)
                    game_state.turn_phase = TurnPhase.PlayerOneMain
                elif game_state.turn_phase == TurnPhase.PlayerOneMain:
                    pass  # Progresses forward by player pressing end turn
                elif game_state.turn_phase == TurnPhase.End:
                    game_state.turn_phase = TurnPhase.Standby
                await co_suspend()
        except GeneratorExit:
            await co_return()
