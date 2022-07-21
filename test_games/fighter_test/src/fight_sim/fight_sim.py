from test_games.fighter_test.src.input import *
from test_games.fighter_test.src.hit_box import Attack
from test_games.fighter_test.src.task import *


class FighterStance:
    STANDING = 0
    CROUCHING = 1
    JUMPING = 2


class Fighter:
    def __init__(self, node: Node2D, collider: Collider2D, input_buffer: InputBuffer):
        self.node = node
        self.collider = collider
        self.input_buffer = input_buffer
        self.velocity = Vector2.ZERO()
        self.speed = 50
        self.is_attacking = False  # Temp
        self.stance = FighterStance.STANDING

    def update_input_state(self) -> None:
        self.input_buffer.process_inputs()

    def set_is_attacking(self, value: bool) -> None:
        self.is_attacking = value

    def spawn_basic_attack_from_stance(self) -> Attack:
        attack = Attack.new()
        # TODO: Clean up attack stuff by calculating the sprite with the width and origin x
        x_scale_vec = Vector2(math.copysign(1.0, self.node.scale.x), 1.0)
        # Facing right
        if x_scale_vec.x > 0.0:
            attack_offset = Vector2(48, 0) * x_scale_vec
        # Facing left
        else:
            attack_offset = Vector2(80, 0) * x_scale_vec
        attack.position = self.node.global_position + attack_offset
        return attack


class AttackRef:
    def __init__(self, attack: Attack, update_task: Coroutine, fighter_index: int):
        self.attack = attack
        self.update_task = update_task
        self.fighter_index = fighter_index


class FighterSimulation:
    def __init__(self, main_node: Node2D):
        self.main_node = main_node
        self.fighters = []
        self.network_receiving_fighters = []
        self.active_attacks = []

    def add_fighter(self, fighter: Fighter) -> None:
        self.fighters.append(fighter)
        if isinstance(fighter.input_buffer, NetworkReceiverInputBuffer):
            self.network_receiving_fighters.append(fighter)

    def add_attack(self, attack: Attack, fighter_index: int) -> None:
        self.active_attacks.append(
            AttackRef(
                attack=attack,
                update_task=attack.update_task(0.1),
                fighter_index=fighter_index,
            )
        )

    def update(self, delta_time: float) -> None:
        # Move fighters
        for i, fighter in enumerate(self.fighters):
            fighter.input_buffer.process_inputs()
            if not fighter.is_attacking:
                if fighter.input_buffer.move_left_pressed:
                    fighter.velocity += Vector2.LEFT()
                elif fighter.input_buffer.move_right_pressed:
                    fighter.velocity += Vector2.RIGHT()

                if fighter.velocity != Vector2.ZERO():
                    delta_vector = Vector2(
                        fighter.speed * delta_time, fighter.speed * delta_time
                    )
                    fighter.node.add_to_position(fighter.velocity * delta_vector)
                    fighter.velocity = Vector2.ZERO()

            # Attack
            if fighter.input_buffer.light_punch_pressed and not fighter.is_attacking:
                attack = fighter.spawn_basic_attack_from_stance()
                print(f"[PY_SCRIPT] attack = {attack}")
                self.main_node.add_child(attack)
                self.add_attack(attack=attack, fighter_index=i)
                fighter.is_attacking = True

        # Kill inputs on network input buffers
        for receiver_fighter in self.network_receiving_fighters:
            receiver_fighter.input_buffer.kill_inputs()

        # Collision test, assumes two fighters for now.  TODO: Clean up later
        collided_entities = CollisionHandler.process_collisions(
            self.fighters[0].collider
        )
        for entity in collided_entities:
            print(f"Entities collided!")
            break

        # Attack test
        for attack_ref in self.active_attacks:
            try:
                awaitable = attack_ref.update_task.send(None)
                if awaitable.state == Awaitable.State.FINISHED:
                    raise StopIteration
            except StopIteration:
                self.fighters[attack_ref.fighter_index].is_attacking = False
                attack_ref.attack.queue_deletion()
                self.active_attacks.remove(attack_ref)
                continue

    def on_entities_collided(
        self, collider: Collider2D, collided_entities: list
    ) -> None:
        pass

    def network_update(self, message: str) -> None:
        # print(f"net update! message: '{message}'")
        if self.network_receiving_fighters:
            if message.startswith("lm:"):
                input_datum = message.split(",")
                for input_data in input_datum:
                    input_name, input_value = input_data.split(":")
                    if input_name == "lm":
                        self.network_receiving_fighters[
                            0
                        ].input_buffer.move_left_pressed = (input_value == "True")
                    elif input_name == "rm":
                        self.network_receiving_fighters[
                            0
                        ].input_buffer.move_right_pressed = (input_value == "True")
                    # print(f"input_name: {input_name}, input_value: {input_value}")
