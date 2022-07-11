import time
from typing import Coroutine, Callable


class Awaitable:
    class State:
        SUSPENDED = 0
        FINISHED = 1

    def __init__(self, state: int):
        self.state = state

    def __await__(self):
        yield self


class Task:
    def __init__(self, coroutine: Coroutine):
        self.coroutine = coroutine


class TaskManager:
    pass


def co_suspend() -> Awaitable:
    return Awaitable(Awaitable.State.SUSPENDED)


def co_return() -> Awaitable:
    return Awaitable(Awaitable.State.FINISHED)


async def co_wait_until(predicate: [Callable, Coroutine]):
    if isinstance(predicate, Callable):
        while not predicate():
            await co_suspend()
    elif isinstance(predicate, Coroutine):
        try:
            while predicate.send(None).state == Awaitable.State.SUSPENDED:
                await co_suspend()
        except StopIteration:
            pass
    else:
        raise Exception(f"Didn't pass in a Callable or Coroutine into co_wait_until! predicate = {str(predicate)}")


async def co_wait_seconds(seconds: float):
    start_time = time.time()
    while True:
        current_time = time.time()
        delta = current_time - start_time
        if delta >= seconds:
            break
        await co_suspend()
