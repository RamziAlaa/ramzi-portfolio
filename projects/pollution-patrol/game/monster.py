import core.app
import pygame
import random
import enum
import game.events
import core.time

from game.entity import Entity
from core.time import Task, Time
from pygame.math import *
from game.bullet import Bullet
from game.player import Player


MOVE_DIR_LEFT = -1
MOVE_DIR_RIGHT = 1


class MonsterState(enum.Enum):
    SPAWNING = 1
    NORMAL = 2
    DEAD = 3


class Monster(Entity):
    FIRE_COOLDOWN = 1800

    _current_monsters = []

    def __init__(self, level_info, fixed_spawn = False):
        super().__init__(level_info)
        self._current_monsters.append(self)
        self.state = MonsterState.SPAWNING
        self.size = Vector2(128, 100)
        self.pos.x = core.app.screen_width / 2 - (self.size.x / 2)
        self.move_dir = MOVE_DIR_LEFT
        self.image = pygame.image.load("assets/monster.png")
        self.speed = 0.2
        self.pos.y = self.size.y
        if not fixed_spawn:
            self.pos.y += (random.randint(0, 1)* self.size.y)
            self.pos.x += random.uniform(-self.size.x / 2, self.size.x / 2)
        self._bullet_speed = 0.6
        # Cooldowns
        self._elapsed_fire_cooldown_ms = 0
        self._play_spawn_anim()
    
    def _update_movement(self):
        self.pos.x += self.move_dir * self.speed * Time.get_delta_time()
        if self.pos.x < 0:
            self.move_dir = MOVE_DIR_RIGHT
        if self.pos.x > core.app.screen_width - self.size.x:
            self.move_dir = MOVE_DIR_LEFT
        self.enforce_bounds()

    def _fire_bullet(self):
        self.enforce_bounds()
        bullet_spawn_pos = self.pos + Vector2(self.size.x/2, self.size.y)
        bullet = Bullet(self.play_state, bullet_spawn_pos, self)
        bullet.speed = self._bullet_speed
        bullet.pos.x -= bullet.size.x/2
        self.play_state.entities.append(bullet)

    def _play_spawn_anim(self):
        def fade_out():
            if self.state == MonsterState.SPAWNING:
                self.color = (255, 255, 255, 60) 
        def fade_in():
            if self.state == MonsterState.SPAWNING:
                self.color = (255, 255, 255, 180) 
        def set_state():
            if self.state == MonsterState.SPAWNING:
                self.color = (255, 255, 255, 255)
                self.state = MonsterState.NORMAL
        fade_out()
        core.time.add_task(Task(fade_in, 400))
        core.time.add_task(Task(fade_out, 800))
        core.time.add_task(Task(fade_in, 1200))
        core.time.add_task(Task(fade_out, 1600))
        core.time.add_task(Task(set_state, 2000))

    def death_fall(self):
        self.color = (self.color[0], self.color[1], self.color[2], clamp(self.color[3] - 0.2 * Time.get_delta_time(), 0, 255))
        self.pos.y += self.speed * 2 * Time.get_delta_time()
        if self.pos.y > core.app.screen_height or self.color[3] == 0:
            self.queue_free()

    def die(self):
        self.color = (255, 255, 255, 100)
        self.rotation = 180
        self.state = MonsterState.DEAD
        if self in self._current_monsters:
            self._current_monsters.remove(self)
        for monster in self._current_monsters:
            if monster.state == MonsterState.SPAWNING and monster in self._current_monsters:
                self._current_monsters.remove(monster)
                monster.die()

    def update(self):
        if self.state == MonsterState.SPAWNING:
            pass
        elif self.state == MonsterState.NORMAL:
            self._update_movement()
            self._elapsed_fire_cooldown_ms += Time.get_delta_time()
            if self._elapsed_fire_cooldown_ms >= Monster.FIRE_COOLDOWN:
                self._elapsed_fire_cooldown_ms = 0
                self._fire_bullet()
        elif self.state == MonsterState.DEAD:
            self.death_fall()

    def on_collide(self, collisions):
        ent: Entity
        for ent in collisions:
            if type(ent) is Bullet:
                bullet: Bullet = ent
                if type(bullet.owner) is Player and self.state == MonsterState.NORMAL:
                    bullet.queue_free()
                    self.die()
                    if not self.play_state.player_moved:
                        # Spawn another monster during start phase if the player
                        # hasn't moved
                        self.play_state.entities.append(Monster(self.play_state, True))
                    else:
                        self.play_state.handle_monster_death()