import random
import core.app
import pygame
import math

from pygame import Color, Rect, Surface
from core.time import Time


class Star:
    def __init__(self):
        self.rect = Rect(random.randint(0, core.app.screen_width), random.randint(0, core.app.screen_height), 3, 3)
        self.randomize_speed()
        self.randomize_color()
    
    def randomize_speed(self):
        self.speed = random.randint(3, 5)

    def randomize_color(self):
        value = random.randint(50, 100)
        self.color = Color(value, value, value, 255)
    
    def reset_position(self):
        self.rect.x = random.randint(0, core.app.screen_width)
        self.rect.y = 0

    def update(self):
        self.rect.y += self.speed * Time.get_delta_time() / 10


class SpaceCircle:
    def __init__(self, radius, pos, color=Color(255,255,255,255)):
        self.color = color
        self.radius = radius
        self.pos = pos
        
    def draw(self, screen: Surface):
        # temp_surface = Surface((self.radius*2, self.radius*2), pygame.SRCALPHA)
        pygame.draw.circle(screen, self.color, self.pos, self.radius)
        # rect = temp_surface.get_rect()
        # rect.x = self.pos[0] - self.radius
        # rect.y = self.pos[1] - self.radius
        # screen.blit(temp_surface, rect)


class SpaceBackground:
    def __init__(self, star_amount):
        self.stars = []
        self.main_circle = SpaceCircle(
            100, 
            (core.app.screen_width/2, 0),
            (14,14,14,255))
        self.back_circle = SpaceCircle(
            100, 
            (core.app.screen_width/2, 0),
            (10,10,10,255))
        # self.back_circle = SpaceCircle(
        #     100, 
        #     (core.app.screen_width/2, core.app.screen_height),
        #     (255,255,255,10))
        for i in range(0, star_amount):
            self.stars.append(Star())

    
    def update(self):
        # Update stars
        star: Star
        for star in self.stars:
            star.update()
            if star.rect.y > core.app.screen_height:
                star.randomize_speed()
                star.randomize_color()
                star.reset_position()
        # Update circles
        self.main_circle.radius = math.sin(pygame.time.get_ticks()/1000)*100+500
        self.back_circle.radius = math.sin(pygame.time.get_ticks()/1000 + 100)*100+600
        # self.back_circle.update()

    def draw(self, screen: Surface):
        star: Star
        # self.back_circle.draw(screen)
        self.back_circle.draw(screen)
        self.main_circle.draw(screen)
        for star in self.stars:
            pygame.draw.rect(screen, star.color, star.rect)