from typing import Tuple

from test_games.fighter_test.src.input import *
from test_games.fighter_test.src.hit_box import Attack
from test_games.fighter_test.src.task import *


class FighterStance:
    NONE = -1
    STANDING = 0
    CROUCHING = 1
    IN_AIR = 2


class Fighter:
    def __init__(self, node: Node2D, collider: Collider2D, input_buffer: InputBuffer):
        self.node = node
        self.collider = collider
        self.input_buffer = input_buffer
        self.velocity = Vector2.ZERO()
        self.speed = 50
        self.is_attacking = False  # Temp
        self.stance = FighterStance.NONE
        self._previous_stance = FighterStance.NONE

    def update_input_state(self) -> None:
        self.input_buffer.process_inputs()

    def set_is_attacking(self, value: bool) -> None:
        self.is_attacking = value

    def _get_base_attack_offset_and_xscale(self) -> Tuple[Vector2, Vector2]:
        # TODO: Clean up attack stuff by calculating the sprite with the width and origin x
        x_scale_vec = Vector2(math.copysign(1.0, self.node.scale.x), 1.0)
        # Facing left
        if x_scale_vec.x < 0.0:
            return Vector2(80, -10) * x_scale_vec, x_scale_vec
        # Facing right
        return Vector2(48, -10) * x_scale_vec, x_scale_vec

    def spawn_basic_attack_from_stance(self) -> Attack:
        attack_offset, x_scale_vec2 = self._get_base_attack_offset_and_xscale()
        if self.stance == FighterStance.STANDING:
            pass
        elif self.stance == FighterStance.CROUCHING:
            attack_offset += Vector2(0.0, 20) * x_scale_vec2
        elif self.stance == FighterStance.IN_AIR:
            pass
        attack = Attack.new()
        attack.position = self.node.global_position + attack_offset
        return attack

    def set_stance(self, stance: str) -> bool:
        self._previous_stance = self.stance
        self.stance = stance
        return self._previous_stance != self.stance

    async def manage_jump_state(self, delta_time: float):
        jump_height = 10
        floor_y = self.node.position.y
        try:
            while True:
                if jump_height > 0:
                    jump_height -= 1
                    self.node.add_to_position(Vector2.UP())
                else:
                    if self.node.position.y >= floor_y:
                        raise GeneratorExit
                    self.node.add_to_position(Vector2.DOWN())
                await co_suspend()
        except GeneratorExit:
            self.set_stance(FighterStance.STANDING)
            self.node.play("walk")


class AttackRef:
    def __init__(self, attack: Attack, update_task: Coroutine, fighter_index: int):
        self.attack = attack
        self.update_task = update_task
        self.fighter_index = fighter_index


class TimedFunction:
    def __init__(self, exec_time: float, func: Callable):
        self.time = exec_time
        self.func = func
        self.current_time = exec_time
        self.has_been_stopped = False

    def reset(self) -> None:
        self.current_time = self.time
        self.has_been_stopped = False

    def tick(self, delta_time: float) -> bool:
        if self.has_been_stopped:
            return False
        self.current_time -= delta_time
        if self.current_time <= 0.0:
            self.func()
            self.has_been_stopped = True
            return True
        return False


class FighterSimulation:
    def __init__(self, main_node: Node2D):
        self.main_node = main_node
        self.fighters = []
        self.network_receiving_fighters = []
        self.active_attacks = []
        self.timed_funcs = []
        self.fighter_coroutines = []  # temp for now

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

    def add_timed_func(self, timed_func: TimedFunction) -> None:
        self.timed_funcs.append(timed_func)

    def set_fighter_stance(
        self, stance: int, fighter_index: int, anim_name: str
    ) -> None:
        self.fighters[fighter_index].set_stance(stance)
        self.fighters[fighter_index].node.play(anim_name)

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

            # Jump
            if fighter.input_buffer.jump_pressed and not fighter.is_attacking:
                if fighter.set_stance(FighterStance.IN_AIR):
                    fighter.node.play("jump")
                    self.fighter_coroutines.append(
                        fighter.manage_jump_state(delta_time)
                    )

            # Stance logic, super basic now as there are no hit reactions, down states, etc...
            if fighter.stance != FighterStance.IN_AIR:
                if fighter.input_buffer.crouch_pressed:
                    if fighter.set_stance(FighterStance.CROUCHING):
                        fighter.node.play("crouch")
                else:
                    if fighter.set_stance(FighterStance.STANDING):
                        fighter.node.play("walk")

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
        for attack_ref in self.active_attacks[:]:
            try:
                awaitable = attack_ref.update_task.send(None)
                if awaitable.state == Awaitable.State.FINISHED:
                    raise StopIteration
            except StopIteration:
                self.fighters[attack_ref.fighter_index].is_attacking = False
                attack_ref.attack.queue_deletion()
                self.active_attacks.remove(attack_ref)
                continue

        # Handle Timed Funcs with copied list
        for timed_func in self.timed_funcs[:]:
            if timed_func.tick(delta_time):
                self.timed_funcs.remove(timed_func)

        # Coroutines
        for coroutine in self.fighter_coroutines:
            try:
                awaitable = coroutine.send(None)
                if awaitable.state == Awaitable.State.FINISHED:
                    raise StopIteration
            except StopIteration:
                self.fighter_coroutines.remove(coroutine)
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
