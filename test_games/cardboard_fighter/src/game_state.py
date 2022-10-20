from enum import Enum


class GameMode(str, Enum):
    NONE = "none"
    LOCAL_AI = "local_ai"
    LOCAL_PVP = "local_pvp"
    ONLINE_PVP_HOST = "online_pvp_host"
    ONLINE_PVP_CLIENT = "online_pvp_client"


class GameState:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = object.__new__(cls)
            cls.mode = GameMode.NONE
            cls.last_win_state = None
        return cls._instance
