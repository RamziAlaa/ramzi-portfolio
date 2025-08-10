import pygame
import pygame.font

from pygame.font import Font
from pygame import Color, Surface

class TextElement():
    def __init__(self, text: str, is_centered = False, x = 0, y = 0, size = 18, color = Color(255,255,255)):
        self.size = size
        # This is a COLOR class but Python insists it becomes a TUPLE.
        # So I can't just directly reference the Color.a variable like
        # a sane human being and have to use monkey logic and settle with
        # self.color[3]. I'm leaving these type specifiers out of sheer
        # spite, I don't care if this is bad practice, making a Color a
        # tuple for no reason is bad design.
        self.color: Color = Color(color)
        self.font = pygame.font.SysFont('sans', self.size)
        self.is_centered = is_centered
        self.text = text
        self.x = x
        self.y = y

    def draw(self, screen: Surface, x_offset: int = 0, y_offset: int = 0):
        text_surface = self.font.render(self.text, False, self.color)
        text_surface.set_alpha(self.color[3])
        text_rect = text_surface.get_rect()
        text_rect = text_rect.move(self.x + x_offset, self.y + y_offset)
        if self.is_centered:
            text_rect.x -= text_rect.width/2
        screen.blit(text_surface, text_rect)

class TextGroup():
    def __init__(self, text_elements: list = [], extra_spacing: int = 10, x = 0, y = 0):
        self._text_elements = []
        self._next_offset = 0
        self.x = x
        self.y = y
        self._extra_spacing = extra_spacing
        for element in text_elements:
            self.append(element)

    # def __eq__(self, value):
    #     if type(value) is TextGroup:
    #         self = TextGroup
    #     elif type(value) is Iterable:
    #         for element in value:
    #           pass
    #     else:
    #         raise TypeError()

    def append(self, text_element: TextElement):
        if len(self._text_elements) == 0:
            self._text_elements.append(text_element)
            return
        last_element: TextElement = self._text_elements[len(self._text_elements) - 1]
        self._next_offset += last_element.size + self._extra_spacing
        text_element.y += self._next_offset
        self._text_elements.append(text_element)
    
    def draw(self, screen):
        element: TextElement
        for element in self._text_elements:
            element.draw(screen, x_offset=self.x, y_offset=self.y)
    
    def get_text_elements(self):
        return self._text_elements