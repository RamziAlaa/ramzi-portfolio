import pygame
import core.app

from pygame.math import *
from pygame import Color, Rect
from pygame import Surface
from game.play_state import PlayState

class Entity:
    def __init__(self, level_info: PlayState):
        self.play_state = level_info
        self.color = Color(255, 255, 255, 255)
        self.pos = Vector2(300, 0)
        self.size = Vector2(50, 50)
        self.rotation = 0
        self._queued_for_free = False
        self.image: Surface

    # Important function calls handled by Level.
    # Overloading not recommended unless you know
    # what you're doing.
    def pre_update(self):
        # Simple collision checking. Every entity will check all other 
        # entities if its colliding. This is a brute force approach to
        # collision checking and has a TOTAL time complexity of O(n^2)
        # (when handled by Level). Okay for now but can result in slowdown in 
        # longer play sessions.
        rect = self.get_rect()
        ent: Entity
        collisions = []
        for ent in self.play_state.entities:
            if rect.colliderect(ent.get_rect()) and ent != self:
                collisions.append(ent)
        self.on_collide(collisions)

    # Virtual update function. Overload to implement
    # logic that runs every frame.
    def update(self):
        pass

    # Virtual function. Called when the entity collides with
    # something.
    def on_collide(self, collisions: list):
        pass

    # Ensures the entity doesn't escape the level's bounds.
    def enforce_bounds(self):
        self.pos.x = clamp(self.pos.x, 0, core.app.screen_width - self.size.x)

    # Virtual input function. Runs every time the user inputs
    # something.
    def input(self, key_pressed: pygame.key.ScancodeWrapper):
        pass

    # Tells Level that the entity wishes to be freed. Garbage
    # collection will take care of it soon after.
    def queue_free(self):
        self._queued_for_free = True
    
    def get_queued_for_free(self):
        return self._queued_for_free
    
    def get_rect(self) -> Rect:
        return Rect(self.pos.x, self.pos.y, self.size.x, self.size.y)