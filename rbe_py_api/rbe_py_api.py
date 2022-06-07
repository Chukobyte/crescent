import rbe_py_api_internal


class AudioSource:
    def __init__(self, file_path: str):
        self.file_path = file_path


class Texture:
    def __init__(
        self, file_path: str, wrap_s: str, wrap_t: str, filter_min: str, filter_mag: str
    ):
        self.file_path = file_path
        self.wrap_s = wrap_s
        self.wrap_t = wrap_t
        self.filter_min = filter_min
        self.filter_mag = filter_mag


class Font:
    def __init__(self, file_path: str, uid: str, size: int):
        self.file_path = file_path
        self.uid = uid
        self.size = size


class InputAction:
    def __init__(self, name: str, values: list):
        self.name = name
        self.values = values


def configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=600,
    target_fps=66,
):
    rbe_py_api_internal.configure_game(
        game_tile=game_tile,
        window_width=window_width,
        window_height=window_height,
        resolution_width=resolution_width,
        resolution_height=resolution_height,
        target_fps=target_fps,
    )


def create_assets(audio_sources=None, textures=None, fonts=None):
    if fonts is None:
        fonts = []
    if textures is None:
        textures = []
    if audio_sources is None:
        audio_sources = []
    rbe_py_api_internal.create_assets(
        audio_sources=audio_sources, textures=textures, fonts=fonts
    )


def configure_inputs(input_actions=None):
    if input_actions is None:
        input_actions = []
    rbe_py_api_internal.configure_inputs(input_actions=input_actions)
