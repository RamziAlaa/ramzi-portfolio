import pygame
import core.time
import asyncio

from pygame import Vector2
from game.level import Level
from core.time import Time
from game.player import Player
from game.monster import Monster


screen_width = 600
screen_height = 900
mouse_move_toggle = True


class App:
    def __init__(self):
        self._screen = pygame.display.set_mode((screen_width, screen_height), pygame.SRCALPHA)
        self._level = Level(Vector2(screen_width, screen_height))
        self._is_running = True
        self._reset_pressed = False
        self._mouse_move_toggle_pressed = False
    
    async def run_main_loop(self):
        # Main Loop
        while self._is_running:
            # Event handling
            events = pygame.event.get()
            for event in events:
                # Window handling
                if event.type == pygame.QUIT:
                    self._is_running = False
                    break
            core.time.fire_tasks()
            self.update()
            self.draw()
            await asyncio.sleep(0)
            Time.tick()
    
    def get_is_running(self) -> bool:
        return self._is_running
    
    def get_level(self) -> Level:
        return self._level

    def update(self):
        keys_pressed = pygame.key.get_pressed()
        mb_pressed = pygame.mouse.get_pressed()
        # Reset
        if (keys_pressed[pygame.K_r] or mb_pressed[2]) and not self._reset_pressed:
            self._reset_pressed = True
            if not Time.is_paused:
                self._level.reset()
        if not (keys_pressed[pygame.K_r] or mb_pressed[2]) and self._reset_pressed:
            self._reset_pressed = False
        
        # Mouse movement
        if keys_pressed[pygame.K_m] and not self._mouse_move_toggle_pressed:
            mouse_move_toggle = not mouse_move_toggle
            self._mouse_move_toggle_pressed = True
            print("Mouse movement toggle: %s" % mouse_move_toggle)
        if not keys_pressed[pygame.K_m] and self._mouse_move_toggle_pressed:
            self._mouse_move_toggle_pressed = False
        # Update level
        self._level.update()

    def draw(self):
        self._screen.fill((0, 0, 0))           # Clear the screen.
        self._level.draw(self._screen)
        pygame.display.update()
    
