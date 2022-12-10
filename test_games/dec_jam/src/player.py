from crescent_api import *
from src.task import *


class Timer:
    def __init__(self, time: float):
        self.time = time
        self.time_remaining = time

    def tick(self, delta_time: float):
        self.time_remaining = max(self.time_remaining - delta_time, 0.0)
        return self

    def reset(self) -> None:
        self.time_remaining = self.time


class Attack(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 1.0
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
            stand_size = Size2D(48, 96)
            stand_pos = Vector2.ZERO() + Vector2(-24, -48)
            self.color_rect.size = stand_size
            self.color_rect.position = stand_pos
            self.collider.extents = stand_size
            self.collider.position = stand_pos
        elif stance == PlayerStance.CROUCHING:
            crouch_size = Size2D(48, 48)
            crouch_pos = Vector2(0, 48) + Vector2(-24, -48)
            self.color_rect.size = crouch_size
            self.color_rect.position = crouch_pos
            self.collider.extents = crouch_size
            self.collider.position = crouch_pos
        elif stance == PlayerStance.IN_AIR:
            pass

    # Tasks
    async def physics_update_task(self):
        # Doesn't change so no need to get every frame
        engine_delta_time = Engine.get_global_physics_delta_time()
        try:
            while True:
                delta_time = self.get_full_time_dilation() * engine_delta_time
                input_dir = Vector2.ZERO()
                if Input.is_action_pressed(name="move_left"):
                    input_dir = Vector2.LEFT()
                elif Input.is_action_pressed(name="move_right"):
                    input_dir = Vector2.RIGHT()

                if input_dir != Vector2.ZERO():
                    if self.stance != PlayerStance.CROUCHING:
                        self.add_to_position(
                            input_dir
                            * Vector2(delta_time * self.speed, delta_time * self.speed)
                        )
                    self.direction_facing = input_dir

                if self.stance != PlayerStance.IN_AIR:
                    if Input.is_action_pressed(name="crouch"):
                        if self.stance != PlayerStance.CROUCHING:
                            self._update_stance(PlayerStance.CROUCHING)
                    else:
                        if self.stance == PlayerStance.CROUCHING:
                            self._update_stance(PlayerStance.STANDING)
                else:
                    if Input.is_action_pressed(name="jump"):
                        self.stance = PlayerStance.IN_AIR
                await co_suspend()
        except GeneratorExit:
            pass
