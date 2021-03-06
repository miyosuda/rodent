# -*- coding: utf-8 -*-
import numpy as np
import pygame, sys
from pygame.locals import *

from nav_maze_static_environment import NavMazeStaticEnvironment

BLACK = (0, 0, 0)


class RandomAgent(object):
    def __init__(self, action_num):
        self.action_num = action_num

    def choose_action(self, state):
        return np.random.randint(self.action_num)


class Display(object):
    def __init__(self, display_size):
        self.width = display_size[0]
        self.height = display_size[1]

        self.env = NavMazeStaticEnvironment(
            width=self.width, height=self.height)
        self.agent = RandomAgent(self.env.get_action_size())

        pygame.init()

        self.surface = pygame.display.set_mode(display_size, 0, 24)
        pygame.display.set_caption('rodentia')

        self.last_state = self.env.reset()

    def update(self):
        self.surface.fill(BLACK)
        self.process()
        pygame.display.update()

    """
    def get_manual_action(self):
        pressed = pygame.key.get_pressed()

        if pressed[K_q]:
            return 0
        if pressed[K_e]:
            return 1
        if pressed[K_a]:
            return 2
        if pressed[K_d]:
            return 3
        if pressed[K_w]:
            return 4
        if pressed[K_s]:
            return 5
        return -1
    """

    def process(self):
        #action = self.get_manual_action()
        action = self.agent.choose_action(self.last_state)

        state, reward, terminal = self.env.step(action=action)

        if reward != 0:
            print("reward={}".format(reward))

        image = pygame.image.frombuffer(state, (self.width, self.height),
                                        'RGB')
        self.surface.blit(image, (0, 0))

        self.last_state = state

        if terminal:
            self.last_state = self.env.reset()


def main():
    display_size = (256, 256)
    display = Display(display_size)
    clock = pygame.time.Clock()

    running = True
    FPS = 60

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    running = False

        display.update()
        clock.tick(FPS)


if __name__ == '__main__':
    main()
