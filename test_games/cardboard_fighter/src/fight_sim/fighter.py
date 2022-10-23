from typing import Tuple

from crescent_api import *
from src.attack import Attack, DragonFireBallAttack, DragonPunchAttack, DragonPunchKick
from src.special_moves import SpecialMovesManager
from src.task import co_suspend
from src.health_bar import HealthBar
from src.input import InputBuffer
from src.timer import Timer


class FighterStance:
    NONE = -1
    STANDING = 0
    CROUCHING = 1
    IN_AIR = 2


class FighterState:
    IDLE = 0
    ATTACKING = 1
    BLOCKING = 2


class Fighter:
    def __init__(
        self,
        node: AnimatedSprite,
        collider: Collider2D,
        input_buffer: InputBuffer,
        health_bar: HealthBar,
    ):
        self.node = node
        self.collider = collider
        self.input_buffer = input_buffer
        self.health_bar = health_bar
        self.velocity = Vector2.ZERO()
        self.hp = 100
        self.speed = 50
        self.state = FighterState.IDLE
        self.stance = FighterStance.NONE
        self.facing_dir = Vector2.RIGHT()
        self.moves_manager = SpecialMovesManager()
        self._previous_stance = FighterStance.NONE
        self.block_timer = Timer(1.0)

    def update_input_state(self) -> None:
        self.input_buffer.process_inputs()

    def set_is_attacking(self, value: bool) -> None:
        self.state = FighterState.ATTACKING

    def _get_base_attack_offset_and_xscale(self) -> Tuple[Vector2, Vector2]:
        # TODO: Clean up attack stuff by calculating the sprite with the width and origin x
        x_scale_vec = Vector2(math.copysign(1.0, self.node.scale.x), 1.0)
        # Facing left
        if x_scale_vec.x < 0.0:
            return Vector2(80, -10) * x_scale_vec, x_scale_vec
        # Facing right
        return Vector2(48, -10) * x_scale_vec, x_scale_vec

    def _get_default_attack_position(self) -> Vector2:
        attack_offset, x_scale_vec2 = self._get_base_attack_offset_and_xscale()
        if self.stance == FighterStance.STANDING:
            pass
        elif self.stance == FighterStance.CROUCHING:
            attack_offset += Vector2(0.0, 20) * x_scale_vec2
        elif self.stance == FighterStance.IN_AIR:
            attack_offset += Vector2(0.0, 20) * x_scale_vec2
        return self.node.global_position + attack_offset

    def spawn_basic_attack_from_stance(self) -> Attack:
        attack = Attack.new()
        attack.position = self._get_default_attack_position()
        attack.direction = self.facing_dir
        attack.life_time = 0.2
        return attack

    def spawn_special_attack(self, attack_name) -> Attack:
        if attack_name == "Dragon Fire Ball":
            dragon_fire_ball = DragonFireBallAttack.new()
            dragon_fire_ball.position = self._get_default_attack_position()
            dragon_fire_ball.direction = self.facing_dir
            return dragon_fire_ball
        elif attack_name == "Dragon Uppercut":
            dragon_uppercut = DragonPunchAttack.new()
            dragon_uppercut.position = self._get_default_attack_position()
            dragon_uppercut.direction = self.facing_dir
            return dragon_uppercut
        elif attack_name == "Dragon Kick":
            dragon_kick = DragonPunchKick.new()
            dragon_kick.position = self._get_default_attack_position()
            dragon_kick.direction = self.facing_dir
            return dragon_kick
        return self.spawn_basic_attack_from_stance()

    def on_attack_connect(self, attack: Attack) -> None:
        print(
            f"Attack Node '{attack}' connected to '{self.node}' with collider '{self.collider}'"
        )
        if self._can_block():
            # TODO: Do more stuff for blocking...
            print("Blocked!")
            self.state = FighterState.BLOCKING
            self.block_timer.reset()
        else:
            self.hp = max(self.hp - 10, 0)
            self.health_bar.set_health_percentage(self.hp)

    def _can_block(self) -> bool:
        if self.state != FighterState.IDLE:
            return False
        scale = self.node.scale
        return (self.input_buffer.move_left_pressed and scale.x > 0) or (
            self.input_buffer.move_right_pressed and scale.x < 0
        )

    def set_stance(self, stance: int) -> bool:
        self._previous_stance = self.stance
        self.stance = stance
        return self._previous_stance != self.stance

    def set_state(self, state: int) -> None:
        self.state = state

    async def manage_jump_state(self, delta_time: float):
        jump_height = 40
        up_speed = 10
        down_speed = 5
        floor_y = self.node.position.y
        try:
            while True:
                if jump_height > 0:
                    jump_height -= up_speed
                    self.node.add_to_position(Vector2(0.0, -up_speed))
                else:
                    if self.node.position.y >= floor_y:
                        raise GeneratorExit
                    self.node.add_to_position(Vector2(0.0, down_speed))
                await co_suspend()
        except GeneratorExit:
            self.set_stance(FighterStance.STANDING)
            self.node.play("idle")
