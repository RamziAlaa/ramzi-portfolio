from pygame.time import Clock

class Time:
    _delta_time = 0
    _clock = Clock()

    desired_frame_rate = 60.0
    is_paused = False

    def tick():
        Time._delta_time = Time._clock.tick(Time.desired_frame_rate)
    
    def get_delta_time():
        return Time._delta_time
    
    def get_clock(self):
        return self._clock

class Task:
    def __init__(self, func, time_ms: int):
        self.func = func
        self.time_ms = time_ms
        self.elapsed_ms = 0


_scheduled_tasks: list[Task] = []

def add_task(task: Task):
    _scheduled_tasks.append(task)

def fire_tasks():
    if Time.is_paused:
        return
    task: Task
    for task in _scheduled_tasks:
        task.elapsed_ms += Time.get_delta_time()
        if task.elapsed_ms >= task.time_ms:
            task.func()
            _scheduled_tasks.remove(task)