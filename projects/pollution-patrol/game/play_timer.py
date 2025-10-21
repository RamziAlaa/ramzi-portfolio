import pygame.font
import random
import core.time

from core.time import Task, Time
from pygame.math import *
from pygame import Rect
from pygame import Surface

# from game.monster import Monster


# TODO: Use TextElement for the text
class PlayTimer:
    _elapsed_time_ms = 0

    def __init__(self, level_info, is_centered = False, is_active = False, inactive_msg = ""):
        self.play_state = level_info
        self.font = pygame.font.SysFont('Mono', 30)
        self.x = 0
        self.y = 0
        self.is_centered = is_centered
        self.is_active = is_active
        self.inactive_msg = inactive_msg
        self._time_until_spawn_ms = 7000
        self._reset_spawn_time()

    def _reset_spawn_time(self):
        self._spawn_elapsed_ms = 0

    def update(self):
        if self.is_active:
            self._elapsed_time_ms += Time.get_delta_time()
            # Spawn monsters
            self._spawn_elapsed_ms += Time.get_delta_time()
            if self._spawn_elapsed_ms >= self._time_until_spawn_ms:
                from game.monster import Monster
                self._reset_spawn_time()
                def spawn_monster():
                    if self.is_active:
                        self.play_state.entities.append(Monster(self.play_state))
                spawn_count = 0
                spawn_time_offset = 0
                for i in range(0, int(1 + ((self._elapsed_time_ms/ 1000)/30))):
                    spawn_count += 1
                    if spawn_count >= 3:
                        spawn_count = 0
                        spawn_time_offset += 1000
                    core.time.add_task(Task(spawn_monster, (i * 1000) + spawn_time_offset))

    def reset(self):
        self._elapsed_time_ms = 0
    
    def pause_and_show_messages(self, messages: dict[str, int]):
        self.is_active = False
        time_offset_ms = 0
        for msg in messages.keys():
            if time_offset_ms == 0:
                self.inactive_msg = msg
                time_offset_ms += messages[msg]
            else:
                def show_message(): self.inactive_msg = msg
                core.time.add_task(Task(show_message, time_offset_ms))
                time_offset_ms += messages[msg]
        def reactivate(): self.is_active = True
        core.time.add_task(Task(reactivate, time_offset_ms))

    # Returns time in miliiseconds
    def get_time(self):
        return self._elapsed_time_ms
    
    def get_time_sec(self) -> float:
        return self._elapsed_time_ms / 1000.0

    def draw(self, screen: pygame.Surface):
        text = ""
        if self.is_active:
            text = '{0:.3f}'.format(self.get_time_sec())
        else:
            text = self.inactive_msg
        text_surface = self.font.render(text, False, (255,255,255))
        text_rect = text_surface.get_rect()
        text_rect = text_rect.move(self.x, self.y)
        if self.is_centered:
            text_rect.x -= text_rect.width/2
        screen.blit(text_surface, text_rect)