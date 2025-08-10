#!./venv/bin/python
"""
Usage: main.py [option] ...

Options:
--help      : Prints information about the program.
--profile   : Turns on profiling mode. Only use this for debugging. (The more 
              time it takes for time.py:10(tick) to call, the more frames are
              available. This is good.)
"""

import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"
import pygame
import pygame.font
import sys

from core.app import App


class MainArguments:
    profile_mode = False


def parse_args():
    quit_after_parse = False
    if "--help" in sys.argv:
        print(__doc__)
        quit_after_parse = True
    if "--profile" in sys.argv:
        MainArguments.profile_mode = True
    if quit_after_parse:
        quit(0)
    

def main():
    pygame.display.init()
    pygame.font.init()
    pygame.display.set_caption("Pollution Patrol")
    pygame.display.set_icon(pygame.image.load("./assets/paddle.png"))
    # pygame.mouse.set_visible(False)
    game_instance = App()
    game_instance.run_main_loop()


if __name__ == "__main__":
    parse_args()
    if MainArguments.profile_mode:
        import cProfile
        cProfile.run("main()")
    else:
        main()
