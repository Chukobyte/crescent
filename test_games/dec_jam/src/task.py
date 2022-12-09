import time
from typing import Coroutine, Callable, Optional, List


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
        self.on_close_subscribers = []
        self.valid = True

    def close(self) -> None:
        try:
            self.coroutine.close()
            self.valid = False
            for func in self.on_close_subscribers:
                func(self)
            self.on_close_subscribers.clear()
        except ValueError:
            pass

    def subscribe_to_on_close(self, func: Callable) -> None:
        self.on_close_subscribers.append(func)


class TaskManager:
    def __init__(self, tasks: Optional[List[Task]] = None):
        self.tasks = []
        if tasks:
            for task in tasks:
                self.add_task(task)

    def add_task(self, task: Task) -> None:
        task.subscribe_to_on_close(lambda t: self.remove_task(t))
        self.tasks.append(task)

    def remove_task(self, task: Task) -> None:
        self.tasks.remove(task)

    def update(self) -> None:
        for task in self.tasks[:]:
            if task.valid:
                try:
                    awaitable = task.coroutine.send(None)
                    if awaitable.state == Awaitable.State.FINISHED:
                        raise StopIteration
                except StopIteration:
                    self.remove_task(task)

    def kill_tasks(self) -> None:
        for task in self.tasks[:]:
            if task.valid:
                task.close()


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
        raise Exception(
            f"Didn't pass in a Callable or Coroutine into co_wait_until! predicate = {str(predicate)}"
        )


async def co_wait_seconds(seconds: float):
    start_time = time.time()
    while True:
        current_time = time.time()
        delta = current_time - start_time
        if delta >= seconds:
            break
        await co_suspend()


# Coroutine example
# async def example_coroutine_task():
#     # We should wrap things in try catch
#     try:
#         while True:
#             await co_suspend()
#     # Called if coroutine is cancelled externally
#     except GeneratorExit:
#         pass
#     # Clean up stuff
#     finally:
#         pass
