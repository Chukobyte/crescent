from crescent_api import *


class CardData:
    class ID:
        Thug = "Thug"
        Crook = "Crook"
        Bouncer = "Bouncer"
        Banker = "Banker"
        Politician = "Politician"
        CrookedCop = "CrookedCop"
        Agent = "Agent"
        Royal = "Royal"


# Base definitions for cards
class CardDefinition:
    def __init__(self, id: str, cost: int, power: int):
        self.id = id
        self.cost = cost
        self.power = power


CardDefinitions = {
    CardData.ID.Thug: CardDefinition(id=CardData.ID.Thug, cost=1, power=1),
    CardData.ID.Crook: CardDefinition(id=CardData.ID.Crook, cost=1, power=0),
    CardData.ID.Bouncer: CardDefinition(id=CardData.ID.Bouncer, cost=2, power=2),
    CardData.ID.Banker: CardDefinition(id=CardData.ID.Banker, cost=3, power=3),
    CardData.ID.Politician: CardDefinition(id=CardData.ID.Politician, cost=4, power=5),
    CardData.ID.CrookedCop: CardDefinition(id=CardData.ID.CrookedCop, cost=3, power=4),
    CardData.ID.Agent: CardDefinition(id=CardData.ID.Agent, cost=4, power=4),
    CardData.ID.Royal: CardDefinition(id=CardData.ID.Royal, cost=5, power=6),
}


class Card(ColorRect):
    class Location:
        Hand = "hand"
        Moving = "moving"
        Zone = "zone"

    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.id = None
        self.collider = None
        self.stats_label = None
        self.location = None
        self.power = 0
        self.cost = 0

    def update_stats_label(self) -> None:
        if self.stats_label:
            self.stats_label.text = f"{self.cost}     {self.power}"

    def set_power(self, value: int) -> None:
        self.power = value
        self.update_stats_label()

    @staticmethod
    def generate(card_id: str):
        card_size = Size2D(54.0, 72.0)
        new_card = Card.new()
        new_card.size = card_size
        new_card.color = Color.linear_color(0.73, 0.1, 0.1)
        # Create collider
        collider_padding = 4.0
        new_card.collider = Collider2D.new()
        new_card.collider.position = Vector2(
            -collider_padding / 2.0, -collider_padding / 2.0
        )
        new_card.collider.extents = Size2D(
            card_size.w + collider_padding, card_size.h + collider_padding
        )
        # Create power label
        new_card.stats_label = TextLabel.new()
        new_card.stats_label.font_uid = "verdana-16"
        new_card.update_stats_label()
        # new_card.stats_label.text = str(new_card.power)
        # new_card.stats_label.position = Vector2(card_size.w - 12.0, 12.0)
        new_card.stats_label.position = Vector2(2.0, 14.0)
        # Set card id and definition
        new_card.set_definition(card_id)
        return new_card

    # TODO: Allow queueing entities to add to node when not in scene (within the engine)
    def _start(self) -> None:
        if self.collider:
            self.add_child(self.collider)
        if self.stats_label:
            self.add_child(self.stats_label)

    def set_definition(self, card_id: str) -> None:
        base_def = self.get_base_definition(card_id)
        if base_def:
            self.id = base_def.id
            self.set_power(base_def.power)

    @staticmethod
    def get_base_definition(card_id: str) -> CardDefinition:
        return CardDefinitions.get(card_id, None)
