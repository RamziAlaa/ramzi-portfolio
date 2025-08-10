import pygame
import core.app

from pygame.time import Clock
from pygame.math import Vector2
from game.entity import Entity
from core.time import Time

class Bullet(Entity):
    LIFESPAN_MS = 10000
    BLINK_COLOR = (255, 255, 255, 180)
    NORMAL_COLOR = (255, 255, 255, 255)

    def get_blink_pulse_ms():
        return int(Time.desired_frame_rate) 

    def __init__(self, level, position: Vector2, owner: Entity, direction: Vector2 = Vector2(0, 1)):
        super().__init__(level)
        self.clock = Clock()
        self.speed = 2
        self.pos = position
        self.owner = owner
        self.size = Vector2(16, 16)
        self.elapsed_life_ms = 0
        self.elapsed_blink_ms = 0
        self.is_blinking = False
        self.direction = direction
        self.image = pygame.image.load("assets/bullet.png")
    
    def update(self):
        # All bullets have a lifespan to avoid two things:
        # - Memory leaks
        # - Floating point errors
        self.elapsed_life_ms += Time.get_delta_time()
        if self.elapsed_life_ms >= Bullet.LIFESPAN_MS:
            self.queue_free()
            return
        # Check if we're below the y limit.
        if self.pos.y > core.app.screen_height:
            print("Bullet hit the bottom.")
            self.play_state.decrement_lives()
            self.queue_free()
        # Blink logic
        self.elapsed_blink_ms += Time.get_delta_time()
        if self.elapsed_blink_ms >= Bullet.get_blink_pulse_ms():
            self.elapsed_blink_ms = 0
            if self.is_blinking:
                self.color = Bullet.BLINK_COLOR
            else:
                self.color = Bullet.NORMAL_COLOR
            self.is_blinking = not self.is_blinking
        # Movement
        self.pos += self.direction * self.speed * Time.get_delta_time()
        if self.pos.x < 0 or self.pos.x >= core.app.screen_width - self.size.x:
            self.direction.x = -self.direction.x
        self.enforce_bounds()
        self.rotation = 180*min(-self.direction.y, 0)
