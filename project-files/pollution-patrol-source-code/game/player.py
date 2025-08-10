import pygame
import core.app

from pygame.math import Vector2
from pygame.sprite import Sprite

from game.entity import Entity
from game.bullet import Bullet
from core.time import Time

class Player(Entity):
    def __init__(self, level_info):
        super().__init__(level_info)
        self.speed = 1.2
        self.walk_speed = 0.4
        self.image = pygame.image.load("assets/paddle.png")
        self.size.x = 120
        self.size.y = 40
        if pygame.mouse.get_pos()[0] != 0:
            self.pos.x = pygame.mouse.get_pos()[0]
        else:
            self.pos.x = core.app.screen_width / 2 - (self.size.x / 2)
        self.pos.y = 800
        self.last_mouse_pos = (0, 0)
    
    def input(self, key_pressed):
        speed = self.speed
        if key_pressed[pygame.K_LSHIFT] or key_pressed[pygame.K_RSHIFT]:
            speed = self.walk_speed
        if key_pressed[pygame.K_a]:
            self.pos.x -= speed * Time.get_delta_time()
            self.play_state.player_moved = True
        if key_pressed[pygame.K_d]:
            self.pos.x += speed * Time.get_delta_time()
            self.play_state.player_moved = True
        # if key_pressed[pygame.K_SPACE] == True:
        #     self.fire_bullet()
    
    def update(self):
        # Mouse movement
        mouse_pos = pygame.mouse.get_pos()
        if self.last_mouse_pos[0] != mouse_pos[0] and core.app.mouse_move_toggle:
            if mouse_pos[0] != self.pos.x:
                self.play_state.player_moved = True
            self.pos.x = mouse_pos[0] - self.size.x / 2
            self.last_mouse_pos = mouse_pos
        self.enforce_bounds()
    
    def reflect_bullet(self, bullet: Bullet):
        # Sends the bullet back up.
        # Depending on where the bullet lands on the paddle, the bullet
        # will be sent either left or right.
        tilt_calc = ((bullet.pos.x - (self.pos.x + self.size.x / 2)) + bullet.size.x / 2) / self.size.x * 2
        new_dir = Vector2(tilt_calc, -1)
        bullet.direction = new_dir
        bullet.owner = self

    def on_collide(self, collisions):
        ent: Entity
        for ent in collisions:
            if type(ent) is Bullet:
                self.reflect_bullet(ent)