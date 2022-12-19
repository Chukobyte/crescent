from crescent_api import *
from src.enemy import Enemy
from src.game_master import GameMaster, LEVEL_BOUNDARY
from src.health_bar import HealthBar
from src.utils.task import *
from src.utils.timer import Timer
from src.utils.game_math import map_to_unit_range, Ease, clamp_pos_to_boundary


class Attack(Collider2D):
    def __init__(self, entity_id: int):
        super().__init__(entity_id=entity_id)
        self.life_time = 0.25
        self.targets = []
        self.direction = Vector2.RIGHT()
        self._task_manager = TaskManager(tasks=[Task(self._update_task())])

    def _start(self) -> None:
        collider_size = Size2D(8, 8)
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
                for collider in collisions:
                    collider_parent = collider.get_parent()
                    if issubclass(type(collider_parent), Enemy):
                        collider_parent.queue_deletion()
                        self.queue_deletion()
                await co_suspend()
            self.queue_deletion()
        except GeneratorExit:
            pass

    def _end(self) -> None:
        self._task_manager.kill_tasks()


class LevelCompletionItem(ColorRect):
    def __init__(self, entity_id: int):
        super().__init__(entity_id)
        self.collider = None

    def _start(self) -> None:
        self.collider = Collider2D.new()
        self.collider.extents = self.size
        self.add_child(self.collider)


class PlayerStance:
    STANDING = "standing"
    CROUCHING = "crouching"
    IN_AIR = "in_air"


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
        self.color_rect = None
        self.collider = None
        self.speed = 75
        self.direction_facing = Vector2.RIGHT()
        self.stance = PlayerStance.STANDING
        self._game_master = GameMaster(self)  # Temp
        self._physics_update_task_manager = TaskManager(
            tasks=[Task(self.physics_update_task())]
        )
        self.health_bar = None

    def _start(self) -> None:
        self.color_rect = self.get_child("ColorRect")
        self.collider = self.get_child("Collider2D")
        # Camera
        Camera2D.set_boundary(boundary=LEVEL_BOUNDARY)
        Camera2D.follow_node(node=self)
        # Temp health bar
        self.health_bar = HealthBar.new()
        self.health_bar.set_health_bar_size(Size2D(10, 10))
        self.health_bar.position = self.position + Vector2(-5.0, -5.0)
        self.add_child(self.health_bar)
        # Temp spawn boundary indicator
        level_completion_item = LevelCompletionItem.new()
        level_completion_item.color = Color.linear_color(0.8, 0.1, 0.8)
        level_completion_item.position = Vector2(LEVEL_BOUNDARY.x, self.position.y)
        SceneTree.get_root().add_child(level_completion_item)

    def _update(self, delta_time: float) -> None:
        if Input.is_action_just_pressed(name="quit_game"):
            Engine.exit()

        if Input.is_action_just_pressed(name="attack"):
            new_attack = Attack.new()
            attack_y = 4
            if self.stance == PlayerStance.CROUCHING:
                attack_y = 10
            new_attack.position = (
                self.position
                + self._center_pos()
                + Vector2(4.0, 0.0)
                + Vector2(self.direction_facing.x * 20, attack_y)
            )
            SceneTree.get_root().add_child(new_attack)

    def _physics_update(self, delta_time: float) -> None:
        self._game_master.update(delta_time)
        self._physics_update_task_manager.update()

        # temp
        collisions = CollisionHandler.process_collisions(self.collider)
        for collider in collisions:
            collider_parent = collider.get_parent()
            if issubclass(type(collider_parent), Enemy):
                self.stats.hp -= 10
                self.health_bar.set_health_percentage(self.stats.hp)
                collider_parent.queue_deletion()
                if self.stats.hp <= 0:
                    SceneTree.change_scene(path="scenes/main.cscn")
            elif issubclass(type(collider_parent), LevelCompletionItem):
                SceneTree.change_scene(path="scenes/main.cscn")

    def _update_stance(self, stance: str) -> None:
        if self.stance == stance:
            return None
        self.stance = stance
        if stance == PlayerStance.STANDING:
            stance_size = Size2D(17, 17)
            stance_pos = self._center_pos()
        elif stance == PlayerStance.CROUCHING:
            stance_size = Size2D(17, 8)
            stance_pos = self._center_pos() + Vector2(0, 9)
        elif stance == PlayerStance.IN_AIR:
            stance_size = Size2D(17, 17)
            stance_pos = self._center_pos()
        else:
            print("***ERROR: invalid stance!")
            stance_size = Size2D()
            stance_pos = Vector2.ZERO()
        self.color_rect.size = stance_size
        self.color_rect.position = stance_pos
        self.collider.extents = stance_size
        self.collider.position = stance_pos

    # Offsets the player's position so that the center is the real position
    def _center_pos(self) -> Vector2:
        return Vector2(-8, -8)

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
                        new_pos = self.position + (
                            input_dir
                            * Vector2(delta_time * self.speed, delta_time * self.speed)
                        )
                        new_pos = clamp_pos_to_boundary(new_pos, LEVEL_BOUNDARY)
                        self.position = new_pos
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
                        alpha = Ease.Cubic.ease_out(
                            elapsed_time=timer.time - timer.time_remaining,
                            from_pos=alpha,
                            to_pos=1.0,
                            duration=timer.time,
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
                        alpha = Ease.Cubic.ease_in(
                            elapsed_time=timer.time - timer.time_remaining,
                            from_pos=alpha,
                            to_pos=1.0,
                            duration=timer.time,
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
