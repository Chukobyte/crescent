from crescent_api import Animation, AnimationFrame, Rect2


IDLE_ANIM = Animation(
    name="idle",
    speed=100,
    loops=True,
    frames=[
        AnimationFrame(
            frame=0,
            texture_path="assets/images/player/snowman_stand_idle.png",
            draw_source=Rect2(0.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=1,
            texture_path="assets/images/player/snowman_stand_idle.png",
            draw_source=Rect2(36.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=2,
            texture_path="assets/images/player/snowman_stand_idle.png",
            draw_source=Rect2(72.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=3,
            texture_path="assets/images/player/snowman_stand_idle.png",
            draw_source=Rect2(108.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=4,
            texture_path="assets/images/player/snowman_stand_idle.png",
            draw_source=Rect2(144.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=5,
            texture_path="assets/images/player/snowman_stand_idle.png",
            draw_source=Rect2(180.0, 0.0, 36.0, 23.0),
        ),
    ],
)

CROUCH_ANIM = Animation(
    name="crouch",
    speed=100,
    loops=True,
    frames=[
        AnimationFrame(
            frame=0,
            texture_path="assets/images/player/snowman_crouch_idle.png",
            draw_source=Rect2(0.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=1,
            texture_path="assets/images/player/snowman_crouch_idle.png",
            draw_source=Rect2(36.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=2,
            texture_path="assets/images/player/snowman_crouch_idle.png",
            draw_source=Rect2(72.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=3,
            texture_path="assets/images/player/snowman_crouch_idle.png",
            draw_source=Rect2(108.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=4,
            texture_path="assets/images/player/snowman_crouch_idle.png",
            draw_source=Rect2(144.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=5,
            texture_path="assets/images/player/snowman_crouch_idle.png",
            draw_source=Rect2(180.0, 0.0, 36.0, 23.0),
        ),
    ],
)

STAND_PUNCH_ANIM = Animation(
    name="stand_punch",
    speed=100,
    loops=True,
    frames=[
        AnimationFrame(
            frame=0,
            texture_path="assets/images/player/snowman_stand_punch.png",
            draw_source=Rect2(0.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=1,
            texture_path="assets/images/player/snowman_stand_punch.png",
            draw_source=Rect2(36.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=2,
            texture_path="assets/images/player/snowman_stand_punch.png",
            draw_source=Rect2(72.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=3,
            texture_path="assets/images/player/snowman_stand_punch.png",
            draw_source=Rect2(108.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=4,
            texture_path="assets/images/player/snowman_stand_punch.png",
            draw_source=Rect2(144.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=5,
            texture_path="assets/images/player/snowman_stand_punch.png",
            draw_source=Rect2(180.0, 0.0, 36.0, 23.0),
        ),
    ],
)

CROUCH_PUNCH_ANIM = Animation(
    name="crouch_punch",
    speed=100,
    loops=True,
    frames=[
        AnimationFrame(
            frame=0,
            texture_path="assets/images/player/snowman_crouch_punch.png",
            draw_source=Rect2(0.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=1,
            texture_path="assets/images/player/snowman_crouch_punch.png",
            draw_source=Rect2(36.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=2,
            texture_path="assets/images/player/snowman_crouch_punch.png",
            draw_source=Rect2(72.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=3,
            texture_path="assets/images/player/snowman_crouch_punch.png",
            draw_source=Rect2(108.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=4,
            texture_path="assets/images/player/snowman_crouch_punch.png",
            draw_source=Rect2(144.0, 0.0, 36.0, 23.0),
        ),
        AnimationFrame(
            frame=5,
            texture_path="assets/images/player/snowman_crouch_punch.png",
            draw_source=Rect2(180.0, 0.0, 36.0, 23.0),
        ),
    ],
)
