from crescent_api import *
from src.enemy import Enemy, EnemyAttack
from src.game_master import GameMaster, LEVEL_BOUNDARY, LevelCompletionItem
from src.health_bar import HealthBar
from src.utils.task import *
from src.utils.timer import Timer
from src.utils.game_math import map_to_unit_range, Ease, clamp_pos_to_boundary
from src.player.player_anims import (
    IDLE_ANIM,
    CROUCH_ANIM,
    CROUCH_PUNCH_ANIM,
    STAND_PUNCH_ANIM,
)


class PlayerAttack(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 0.25
        self.targets = []
        self.direction = Vector2.RIGHT()
        self._task_manager = TaskManager(tasks=[Task(self._update_task())])

    def _start(self) -> None:
        collider_size = Size2D(12, 6)
        collider_color = Color(0, 200, 0, 150)
        self.set_extents(collider_size)
        self.set_color(collider_color)
        color_square = ColorRect.new()
        color_square.size = collider_size
        color_square.color = collider_color
        self.add_child(color_square)
        self.z_index = 2

    def _end(self) -> None:
        self._task_manager.kill_tasks()

    def _physics_update(self, delta_time: float) -> None:
        self._task_manager.update()

    async def _update_task(self) -> None:
        engine_delta_time = Engine.get_global_physics_delta_time()
        life_timer = Timer(self.life_time)
        try:
            while life_timer.tick(engine_delta_time).time_remaining > 0:
                collisions = CollisionHandler.process_collisions(self)
                enemies_collided = []
                for collider in collisions:
                    collider_parent = collider.get_parent()
                    if issubclass(type(collider_parent), Enemy):
                        enemies_collided.append(collider_parent)
                if enemies_collided:
                    # Single frame freeze by setting time dilation to 0.0 then back to 1.0
                    World.set_time_dilation(0.0)
                    await co_suspend()
                    World.set_time_dilation(1.0)
                    for enemy in enemies_collided:
                        enemy.queue_deletion()
                    self.queue_deletion()
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


class PlayerState:
    MOVE = "move"  # main
    ATTACKING = "attacking"
    HURT = "hurt"


class QueuedPlayerState:
    def __init__(self, state: str, data=None):
        self.state = state
        self.data = data


class PlayerStats:
    def __init__(self, hp=0):
        self._max_hp = hp
        self._hp = hp

    def reset(self) -> None:
        self._hp = self._max_hp

    @property
    def hp(self) -> int:
        return self._hp

    @hp.setter
    def hp(self, value: int) -> None:
        self._hp = max(value, 0)


class Player(Node2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.stats = PlayerStats(hp=100)
        self.collider = None
        self.anim_sprite = None
        self.speed = 25
        self.direction_facing = Vector2.RIGHT()
        self.stance = PlayerStance.STANDING
        self.state = PlayerState.MOVE
        self._queued_state: QueuedPlayerState | None = None
        self._has_queued_attack = False
        self._game_master = GameMaster(self)  # Temp
        self._physics_update_task_manager = TaskManager(
            tasks=[Task(self.physics_update_task()), Task(self.collision_update_task())]
        )
        self.health_bar = None

    def _start(self) -> None:
        self.collider = self.get_child("Collider2D")
        self.anim_sprite: AnimatedSprite = AnimatedSprite.new()
        self.anim_sprite.add_animation(IDLE_ANIM)
        self.anim_sprite.add_animation(CROUCH_ANIM)
        self.anim_sprite.add_animation(STAND_PUNCH_ANIM)
        self.anim_sprite.add_animation(CROUCH_PUNCH_ANIM)
        self.anim_sprite.position = self._center_pos()
        self.add_child(self.anim_sprite)
        # Camera
        Camera2D.set_boundary(boundary=LEVEL_BOUNDARY)
        Camera2D.follow_node(node=self)
        # Temp health bar
        self.health_bar = HealthBar.new()
        self.health_bar.set_health_bar_size(Size2D(26, 6))
        self.health_bar.position = self.position + Vector2(60.0, 8.0)
        self.health_bar.ignore_camera = True
        self.health_bar.inner_hp_bar.ignore_camera = True
        SceneTree.get_root().add_child(self.health_bar)
        # Temp spawn boundary indicator
        self._game_master.initialize_level()
        # TODO: Get dir of level completion item and face that dir
        self.scale = Vector2(-1, 1)
        self.direction_facing = Vector2.LEFT()

    def _end(self) -> None:
        self._physics_update_task_manager.kill_tasks()

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

        # Work around for '_physics_update' not being called every frame.  TODO: Update inputs in the engine for this...
        if Input.is_action_just_pressed(name="attack") and self._can_attack():
            self._has_queued_attack = True

    def _physics_update(self, delta_time: float) -> None:
        self._game_master.update(delta_time)
        self._physics_update_task_manager.update()

    def _take_damage(self, damage: int) -> None:
        self.stats.hp -= damage
        self.health_bar.set_health_percentage(self.stats.hp)
        if self.stats.hp <= 0:
            SceneTree.change_scene(path="scenes/end_screen.cscn")

    def _can_attack(self) -> bool:
        return self.state == PlayerState.MOVE

    def _update_stance(self, stance: str) -> None:
        if self.stance == stance:
            return None
        self.stance = stance
        collider_offset = Vector2(-2, 0)  # TODO: Store from player instance on start
        if stance == PlayerStance.STANDING:
            stance_size = Size2D(10, 12)
            stance_pos = collider_offset
        elif stance == PlayerStance.CROUCHING:
            stance_size = Size2D(10, 6)
            stance_pos = collider_offset + Vector2(0, 9)
        elif stance == PlayerStance.IN_AIR:
            stance_size = Size2D(10, 12)
            stance_pos = collider_offset
        else:
            print("***ERROR: invalid stance!")
            stance_size = Size2D()
            stance_pos = Vector2.ZERO()
        self.collider.extents = stance_size
        self.collider.position = stance_pos

    # Offsets the player's position so that the center is the real position
    def _center_pos(self) -> Vector2:
        return Vector2(-8, -8)

    def _queue_state_change(self, new_state: QueuedPlayerState) -> None:
        if new_state.state != self.state:
            self._queued_state = new_state

    # Tasks
    async def physics_update_task(self):
        def get_stance_task(stance: str) -> Task:
            if stance == PlayerStance.STANDING:
                return Task(self._stand_stance_task())
            elif stance == PlayerStance.CROUCHING:
                return Task(self._crouch_stance_task())
            return Task(self._in_air_stance_task())

        state_task_manager = TaskManager(tasks=[Task(self._move_state_task())])
        stance_task_manager = TaskManager(tasks=[get_stance_task(self.stance)])

        def process_queued_state_if_exists():
            if self._queued_state:
                state_task_manager.kill_tasks()
                self.state = self._queued_state.state
                if self.state == PlayerState.MOVE:
                    state_task_manager.add_task(Task(self._move_state_task()))
                elif self.state == PlayerState.ATTACKING:
                    state_task_manager.add_task(
                        Task(self._attack_state_task(attack=self._queued_state.data))
                    )
                elif self.state == PlayerState.HURT:
                    state_task_manager.add_task(Task(self._hurt_state_task()))
                self._queued_state = None

        try:
            self.anim_sprite.play(name=IDLE_ANIM.name)
            prev_stance = self.stance
            while True:
                # State
                process_queued_state_if_exists()
                state_task_manager.update()
                # Stance
                if prev_stance != self.stance:
                    stance_task_manager.kill_tasks()
                    stance_task_manager.add_task(get_stance_task(self.stance))
                    prev_stance = self.stance
                stance_task_manager.update()
                await co_suspend()
        except GeneratorExit:
            pass
        finally:
            state_task_manager.kill_tasks()

    # State tasks
    # Stance funcs
    async def _stand_stance_task(self):
        try:
            while True:
                # self.anim_sprite.play(name=IDLE_ANIM.name)
                if Input.is_action_pressed(name="jump"):
                    self._update_stance(PlayerStance.IN_AIR)
                elif Input.is_action_pressed(name="crouch"):
                    self.anim_sprite.play(name=CROUCH_ANIM.name)
                    self._update_stance(PlayerStance.CROUCHING)
                await co_suspend()
        except GeneratorExit:
            pass

    async def _crouch_stance_task(self):
        try:
            while True:
                # self.anim_sprite.play(name=CROUCH_ANIM.name)
                if Input.is_action_pressed(name="jump"):
                    self._update_stance(PlayerStance.IN_AIR)
                elif (
                    not Input.is_action_pressed(name="crouch")
                    and self.state != PlayerState.ATTACKING
                ):
                    self.anim_sprite.play(name=IDLE_ANIM.name)
                    self._update_stance(PlayerStance.STANDING)
                await co_suspend()
        except GeneratorExit:
            pass

    async def _in_air_stance_task(self):
        try:
            engine_delta_time = Engine.get_global_physics_delta_time()
            self.anim_sprite.play(name=IDLE_ANIM.name)
            jump_height = 12
            position_before_jump = self.position
            position_to_jump_to = position_before_jump + Vector2(0, -jump_height)
            jump_time = 1.0
            timer = Timer(jump_time / 2.0)
            # Go Up
            while (
                timer.tick(
                    self.get_full_time_dilation() * engine_delta_time
                ).time_remaining
                > 0
            ):
                alpha = 1.0 - map_to_unit_range(timer.time_remaining, 0.0, timer.time)
                alpha = Ease.Cubic.ease_out(
                    elapsed_time=timer.time - timer.time_remaining,
                    from_pos=alpha,
                    to_pos=1.0,
                    duration=timer.time,
                )
                new_pos = Vector2.lerp(position_before_jump, position_to_jump_to, alpha)
                self.position = new_pos
                await co_suspend()
            # Go Down
            timer.reset()
            while (
                timer.tick(
                    self.get_full_time_dilation() * engine_delta_time
                ).time_remaining
                > 0
            ):
                alpha = 1.0 - map_to_unit_range(timer.time_remaining, 0.0, timer.time)
                alpha = Ease.Cubic.ease_in(
                    elapsed_time=timer.time - timer.time_remaining,
                    from_pos=alpha,
                    to_pos=1.0,
                    duration=timer.time,
                )
                new_pos = Vector2.lerp(position_to_jump_to, position_before_jump, alpha)
                self.position = new_pos
                await co_suspend()
            # Finish landing
            self.position = position_before_jump
            self._update_stance(PlayerStance.STANDING)
            await co_return()
        except GeneratorExit:
            pass

    async def _move_state_task(self):
        def get_stance_task(stance: str) -> Task:
            if stance == PlayerStance.STANDING:
                return Task(self._stand_stance_task())
            elif stance == PlayerStance.CROUCHING:
                return Task(self._crouch_stance_task())
            return Task(self._in_air_stance_task())

        try:
            engine_delta_time = Engine.get_global_physics_delta_time()
            while True:
                delta_time = self.get_full_time_dilation() * engine_delta_time

                # Check for attack first for now
                # if Input.is_action_just_pressed(name="attack"):
                # if Input.is_action_pressed(name="attack"):
                if self._has_queued_attack:
                    self._has_queued_attack = False
                    new_attack = PlayerAttack.new()
                    attack_y = 10
                    if self.stance == PlayerStance.CROUCHING:
                        attack_y += 4
                        self.anim_sprite.play(name=CROUCH_PUNCH_ANIM.name)
                    elif (
                        self.stance == PlayerStance.STANDING
                        or self.stance == PlayerStance.IN_AIR
                    ):
                        self.anim_sprite.play(name=STAND_PUNCH_ANIM.name)

                    new_attack.position = (
                        self.position
                        + self._center_pos()
                        + Vector2(4.0, 0.0)
                        + Vector2(self.direction_facing.x * 20, attack_y)
                    )
                    SceneTree.get_root().add_child(new_attack)
                    self._queue_state_change(
                        QueuedPlayerState(state=PlayerState.ATTACKING, data=new_attack)
                    )
                    await co_return()

                # Check horizontal movement inputs
                input_dir = Vector2.ZERO()
                if Input.is_action_pressed(name="move_left"):
                    input_dir = Vector2.LEFT()
                elif Input.is_action_pressed(name="move_right"):
                    input_dir = Vector2.RIGHT()
                # Determine movement (no air movement for now)
                if input_dir != Vector2.ZERO():
                    if self.stance == PlayerStance.STANDING:
                        new_pos = self.position + (
                            input_dir
                            * Vector2(delta_time * self.speed, delta_time * self.speed)
                        )
                        new_pos = clamp_pos_to_boundary(new_pos, LEVEL_BOUNDARY)
                        self.position = new_pos
                    self.direction_facing = input_dir
                    self.scale = Vector2(self.direction_facing.x, 1.0)

                await co_suspend()
        except GeneratorExit:
            pass

    async def _attack_state_task(self, attack: PlayerAttack):
        try:
            await co_wait_seconds(attack.life_time - 0.05)
            self._queue_state_change(
                new_state=QueuedPlayerState(state=PlayerState.MOVE)
            )
            if self.stance == PlayerStance.CROUCHING:
                self.anim_sprite.play(name=CROUCH_ANIM.name)
            else:
                self.anim_sprite.play(name=IDLE_ANIM.name)
            await co_return()
        except GeneratorExit:
            pass

    async def _hurt_state_task(self):
        try:
            print("***HURT_TASK")
            while True:
                self._queue_state_change(
                    new_state=QueuedPlayerState(state=PlayerState.MOVE)
                )
                await co_return()
        except GeneratorExit:
            pass

    async def collision_update_task(self) -> None:
        try:
            while True:
                collisions = CollisionHandler.process_collisions(self.collider)
                for collider in collisions:
                    if issubclass(type(collider), EnemyAttack):
                        self._take_damage(damage=10)
                        World.set_time_dilation(0.0)
                        await co_suspend()
                        World.set_time_dilation(1.0)
                        collider.queue_deletion()
                    else:
                        collider_parent = collider.get_parent()
                        if issubclass(type(collider_parent), Enemy):
                            self._take_damage(damage=10)
                            World.set_time_dilation(0.0)
                            await co_suspend()
                            World.set_time_dilation(1.0)
                            collider_parent.queue_deletion()
                        elif issubclass(type(collider_parent), LevelCompletionItem):
                            SceneTree.change_scene(path="scenes/main.cscn")
                await co_suspend()
        except GeneratorExit:
            pass
        finally:
            World.set_time_dilation(1.0)
