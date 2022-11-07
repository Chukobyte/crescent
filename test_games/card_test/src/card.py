from crescent_api import *


class CardData:
    class ID:
        CardOne = "Card0"

    def __init__(self, card_node: ColorRect):
        self.node = card_node
        self.is_in_hand = False

    @property
    def name(self) -> str:
        return self.node.name


class Card(ColorRect):
    class Location:
        Hand = "hand"
        Moving = "moving"
        Zone = "zone"

    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.id = None
        self.collider = None
        self.location = None

    @staticmethod
    def generate(card_id: str):
        card_size = Size2D(40.0, 64.0)
        new_card = Card.new()
        new_card.size = card_size
        new_card.color = Color.linear_color(0.73, 0.1, 0.1)
        new_card.id = card_id
        # Create collider
        collider_padding = 4.0
        new_card.collider = Collider2D.new()
        new_card.collider.position = Vector2(
            -collider_padding / 2.0, -collider_padding / 2.0
        )
        new_card.collider.extents = Size2D(
            card_size.w + collider_padding, card_size.h + collider_padding
        )
        return new_card

    # TODO: Allow queueing entities to add to node when not in scene (within the engine)
    def _start(self) -> None:
        if self.collider:
            self.add_child(self.collider)
