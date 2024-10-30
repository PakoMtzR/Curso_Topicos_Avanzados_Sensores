import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

def init():
    glClearColor(0.0, 0.0, 0.0, 1.0)  # Color de fondo negro
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glShadeModel(GL_SMOOTH)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)


def draw_prism():
    # (x,y,z)
    vertices = [
        (1, 0.2, -1),
        (1, -0.2, -1),
        (-1, -0.2, -1),
        (-1, 0.2, -1),
        (1, 0.2, 1),
        (1, -0.2, 1),
        (-1, -0.2, 1),
        (-1, 0.2, 1)
    ]

    edges = (
        (0, 1),(1, 2),(2, 3),(3, 0),    # Aristas de la cara frontal
        (4, 5),(5, 6),(6, 7),(7, 4),    # Aristas de la cara trasera
        (0, 4),(1, 5),(2, 6),(3, 7)     # Aristas que conectan las caras frontal y trasera
    )

    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(vertices[vertex])
    glEnd()

def main():
    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)
    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)
    init()
    
    angle = 0

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glPushMatrix()
        glRotatef(angle, 0, 0, 1)  # Rotar alrededor de los ejes x, y, z
        draw_prism()
        glPopMatrix()

        pygame.display.flip()
        pygame.time.wait(10)

        angle += 0.2  # Incrementa el ángulo para la rotación
        print(angle)
    pygame.quit()
if __name__ == "__main__":
    main()
