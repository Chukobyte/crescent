from crescent_api import *
from src.utils.task import *
from src.utils.timer import Timer
from src.utils.math import map_to_unit_range


class Attack(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 0.25
        self.targets = []
        self.direction = Vector2.RIGHT()
        self._task_manager = TaskManager(tasks=[Task(self._update_task())])

    def _start(self) -> None:
        collider_size = Size2D(32, 32)
        collider_color = Color(0, 200, 0, 150)
        self.set_extents(collider_size)
        self.set_color(collider_color)
        color_square = ColorRect.new()
        color_square.size = collider_size
        color_square.color = collider_color
        self.add_child(color_square)

    def _physics_update(self, delta_time: float) -> None:
        self._task_manager.update()

    async def _update_task(self) -> None:
        engine_delta_time = Engine.get_global_physics_delta_time()
        life_timer = Timer(self.life_time)
        try:
            # TODO: Get delta time and time dilation from engine python api.
            # Might be enough just to have a reference to the node that spawned the attack and get that time dilation
            while life_timer.tick(engine_delta_time).time_remaining > 0:
                collisions = CollisionHandler.process_collisions(self)
                for collision in collisions:
                    if collision.get_parent().name == "TestThing":
                        collision.get_parent().queue_deletion()
                        self.queue_deletion()
                        break
                await co_suspend()
            self.queue_deletion()
        except GeneratorExit:
            pass

    def _end(self) -> None:
        self._task_manager.kill_tasks()


class PlayerStance:
    STANDING = "standing"
    CROUCHING = "crouching"
    IN_AIR = "in_air"


class Player(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.color_rect = None
        self.collider = None
        self.speed = 200
        self.direction_facing = Vector2.RIGHT()
        self.stance = PlayerStance.STANDING
        self._physics_update_task_manager = TaskManager(
            tasks=[Task(self.physics_update_task())]
        )

    def _start(self) -> None:
        self.color_rect = self.get_child("ColorRect")
        self.collider = self.get_child("Collider2D")
        Camera2D.follow_node(node=self)

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

        if Input.is_action_just_pressed(name="attack"):
            new_attack = Attack.new()
            attack_y = 20
            if self.stance == PlayerStance.CROUCHING:
                attack_y = 50
            new_attack.position = (
                self.position
                + Vector2(-16, -48)
                + Vector2(self.direction_facing.x * 50, attack_y)
            )
            self.get_parent().add_child(new_attack)
            print("Attacked")

    def _physics_update(self, delta_time: float) -> None:
        self._physics_update_task_manager.update()

    def _update_stance(self, stance: str) -> None:
        if self.stance == stance:
            return None
        self.stance = stance
        if stance == PlayerStance.STANDING:
            stance_size = Size2D(48, 96)
            stance_pos = Vector2.ZERO() + Vector2(-24, -48)
        elif stance == PlayerStance.CROUCHING:
            stance_size = Size2D(48, 48)
            stance_pos = Vector2(0, 48) + Vector2(-24, -48)
        elif stance == PlayerStance.IN_AIR:
            stance_size = Size2D(48, 96)
            stance_pos = Vector2.ZERO() + Vector2(-24, -48)
        else:
            stance_size = Size2D()
            stance_pos = Vector2.ZERO()
        self.color_rect.size = stance_size
        self.color_rect.position = stance_pos
        self.collider.extents = stance_size
        self.collider.position = stance_pos

    # Tasks
    async def physics_update_task(self):
        # Doesn't change so no need to get every frame
        engine_delta_time = Engine.get_global_physics_delta_time()
        try:
            while True:
                delta_time = self.get_full_time_dilation() * engine_delta_time
                # Check horizontal movement inputs
                input_dir = Vector2.ZERO()
                if Input.is_action_pressed(name="move_left"):
                    input_dir = Vector2.LEFT()
                elif Input.is_action_pressed(name="move_right"):
                    input_dir = Vector2.RIGHT()
                # Determine movement (no air movement for now)
                if input_dir != Vector2.ZERO():
                    if self.stance == PlayerStance.STANDING:
                        self.add_to_position(
                            input_dir
                            * Vector2(delta_time * self.speed, delta_time * self.speed)
                        )
                    self.direction_facing = input_dir
                # Handle player stances
                if self.stance == PlayerStance.STANDING:
                    if Input.is_action_pressed(name="jump"):
                        self._update_stance(PlayerStance.IN_AIR)
                    elif Input.is_action_pressed(name="crouch"):
                        self._update_stance(PlayerStance.CROUCHING)
                elif self.stance == PlayerStance.CROUCHING:
                    if Input.is_action_pressed(name="jump"):
                        self._update_stance(PlayerStance.IN_AIR)
                    elif not Input.is_action_pressed(name="crouch"):
                        self._update_stance(PlayerStance.STANDING)
                elif self.stance == PlayerStance.IN_AIR:
                    jump_height = 48
                    position_before_jump = self.position
                    position_to_jump_to = position_before_jump + Vector2(
                        0, -jump_height
                    )
                    jump_time = 1.0
                    timer = Timer(jump_time / 2.0)
                    # Go Up
                    while timer.tick(engine_delta_time).time_remaining > 0:
                        alpha = 1.0 - map_to_unit_range(
                            timer.time_remaining, 0.0, timer.time
                        )
                        new_pos = Vector2.lerp(
                            position_before_jump, position_to_jump_to, alpha
                        )
                        self.position = new_pos
                        await co_suspend()
                    # Go Down
                    timer.reset()
                    while timer.tick(engine_delta_time).time_remaining > 0:
                        alpha = 1.0 - map_to_unit_range(
                            timer.time_remaining, 0.0, timer.time
                        )
                        new_pos = Vector2.lerp(
                            position_to_jump_to, position_before_jump, alpha
                        )
                        self.position = new_pos
                        await co_suspend()
                    # Finish landing
                    self.position = position_before_jump
                    self._update_stance(PlayerStance.STANDING)
                await co_suspend()
        except GeneratorExit:
            pass
