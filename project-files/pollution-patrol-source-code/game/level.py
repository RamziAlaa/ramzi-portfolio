import pygame
import core.app

from pygame.math import *
from core.time import Time
from game.play_state import PlayState
from game.play_timer import PlayTimer
from game.entity import Entity
from game.space_background import SpaceBackground
from game.player import Player
from game.monster import Monster
from game.text_element import TextElement, TextGroup


class Level:
    def __init__(self, size: Vector2):
        self.play_state = PlayState(size)
        self.space_background = SpaceBackground(50)
        self.pause_text = TextElement("PAUSED - PRESS ESC TO CONTINUE", True, core.app.screen_width/2, core.app.screen_height / 2)
        self.is_pause_key_pressed = False
        self.reset()

    def reset(self):
        self.play_state = PlayState(self.play_state.size)
        self.play_state.entities.append(Player(self.play_state))
        self.play_state.entities.append(Monster(self.play_state, True))

    def update(self):
        keys_pressed = pygame.key.get_pressed()
        if keys_pressed[pygame.K_ESCAPE] and not self.is_pause_key_pressed:
            Time.is_paused = not Time.is_paused
            self.is_pause_key_pressed = True
        if not keys_pressed[pygame.K_ESCAPE] and self.is_pause_key_pressed:
            self.is_pause_key_pressed = False
        
        self.space_background.update()
        if not Time.is_paused:
            self._update_entities(keys_pressed)
            self.play_state.play_timer.update()

    def draw(self, screen: pygame.Surface):
        self.space_background.draw(screen)
        if Time.is_paused:
            self.pause_text.draw(screen)
            return
        self._draw_entities(screen)
        if not self.play_state.has_game_started or self.play_state.is_game_over:
            self.play_state.splash_text.draw(screen)
        self.play_state.play_timer.draw(screen)
    
    def _update_entities(self, keys_pressed):
        ent: Entity
        for ent in self.play_state.entities:
            if ent.get_queued_for_free():
                self.play_state.entities.remove(ent)
                del ent
                continue
            ent.pre_update()
            ent.input(keys_pressed)
            ent.update()
    
    def _draw_entities(self, screen: pygame.Surface):
        ent: Entity
        for ent in self.play_state.entities:
            try:
                # Rotate the image
                image = pygame.transform.rotate(ent.image, ent.rotation)
                # Set color
                image = image.convert_alpha()
                image.fill(ent.color, special_flags=pygame.BLEND_RGBA_MIN)
                # Calculate rect
                img_rect = image.get_rect()
                img_rect.x = ent.pos.x
                img_rect.y = ent.pos.y
                # Draw to screen
                screen.blit(image, img_rect)
            except AttributeError:
                pygame.draw.rect(screen, ent.color, ent.get_rect())