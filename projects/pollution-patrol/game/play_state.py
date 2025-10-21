import core
import random
import game

from pygame import Vector2
from game.play_timer import PlayTimer
from game.text_element import TextElement, TextGroup


class PlayState:
    def __init__(self, size: Vector2):
        self.entities = []
        self.size = size
        self.play_timer = PlayTimer(self, True, inactive_msg="MOVE AND KILL HOSTILE TO START")
        self.play_timer.x = core.app.screen_width/2
        self.play_timer.y = 10
        self.player_moved = False
        self.has_game_started = False
        self.is_game_over = False
        self.lives = 4
        # Get facts from facts.txt
        self.facts = []
        try:
            with open("facts.txt") as facts_file:
                fact_count = 1
                for line in facts_file:
                    # New lines without data are seperators.
                    if line == "\n":
                        fact_count += 1
                        continue
                    line = line.rstrip()
                    if fact_count > len(self.facts):
                        self.facts.append([line])
                    else:
                        self.facts[len(self.facts) - 1].append(line)
        except FileNotFoundError:
            print("Error: Cannot find fact file.")
        self.set_start_splash_text()

    def decrement_lives(self):
        # The player gets a grace period if the timer
        # isn't active
        if not self.play_timer.is_active or not self.has_game_started:
            return
        self.lives -= 1
        print(self.lives)
        if self.lives <= 0:
            print("GAME OVER")
            self.is_game_over = True
            self.play_timer.is_active = False
            self.play_timer.inactive_msg = "GAME OVER"
            self.entities.clear()
            self.set_game_over_splash_text()
            return
        chance_warning = "%i CHANCES LEFT" % self.lives if not self.lives == 1 else "!!! 1 CHANCE LEFT !!!"
        self.play_timer.pause_and_show_messages({
                "BALL FELL": 2000,
                chance_warning: 2000
        })

    def handle_monster_death(self):
        if not self.has_game_started:
            self.has_game_started = True
            self.play_timer.is_active = True
            for ent in self.entities:
                if type(ent) is game.bullet.Bullet:
                    ent.queue_free()

    def set_start_splash_text(self):
        self.splash_text = TextGroup(x=10, y=300)
        self.splash_text.append(TextElement("Welcome to Pollution Patrol!"))
        self.splash_text.append(TextElement("The goal is to survive for as long as you can."))
        self.splash_text.append(TextElement("Use your paddle to reflect balls back into the enemy."))
        self.splash_text.append(TextElement("Don't allow the ball past you. You'll get 3 extra chances."))
        self.splash_text.append(TextElement(""))
        self.splash_text.append(TextElement("Press A to move left. Press D to move right."))
        self.splash_text.append(TextElement("You can also use the mouse to move. Toggle on or off with M"))
        self.splash_text.append(TextElement("Hold SHIFT to slow down."))
        self.splash_text.append(TextElement("Press ESC to pause the game."))
        self.splash_text.append(TextElement("Press R or RIGHT CLICK to restart."))
        element: TextElement
        for element in self.splash_text.get_text_elements():
            element.color = (255, 255, 255, 100)

    def set_game_over_splash_text(self):
        self.splash_text = TextGroup(x=10, y=300)
        self.splash_text.append(TextElement("Score: " + '{0:.3f}'.format(self.play_timer.get_time_sec())))
        self.splash_text.append(TextElement(""))
        self.splash_text.append(TextElement("Tip:"))
        if len(self.facts) != 0:
            fact = random.choice(self.facts)
            for line in fact:
                self.splash_text.append(TextElement(str(line)))
        self.splash_text.append(TextElement(""))
        self.splash_text.append(TextElement("Press R or RIGHT CLICK to restart."))
        element: TextElement
        for element in self.splash_text.get_text_elements():
            element.color = (255, 255, 255, 200)