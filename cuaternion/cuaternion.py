import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import serial
import time

def init():
    glClearColor(0.0, 0.0, 0.0, 1.0)  # Color de fondo negro
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glShadeModel(GL_SMOOTH)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def draw_prism():
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

# def render_text(text, x, y, font, screen):
#     text_surface = font.render(text, True, (255, 255, 255))
#     screen.blit(text_surface, (x, y))

def main():
    try:
        arduino_port = "COM6"  # Reemplaza con el puerto correcto
        baud_rate = 9600
        timeout = 1

        ser = serial.Serial(arduino_port, baud_rate)
        time.sleep(2)  # Espera un poco para que la conexión se establezca

        pygame.init()
        display = (800, 600)
        screen = pygame.display.set_mode(display, DOUBLEBUF | OPENGL)
        pygame.display.set_caption('MPU6050 Cuaternion')

        gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
        glTranslatef(0.0, 0.0, -5)
        init()

        #font = pygame.font.SysFont("monospace", 36)
        data = [0, 0, 0]

        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
            
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').rstrip().split(",")
                print(f"[Received from Arduino]: Pitch:{data[0]}°, Roll:{data[2]}°, Yaw:{data[1]}°")

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

            # # Desactiva el test de profundidad para renderizar texto en 2D
            # glDisable(GL_DEPTH_TEST)

            # screen = pygame.display.get_surface()
            # render_text(f"Pitch: {data[0]}", 10, 10, font, screen)
            # render_text(f"Roll: {data[2]}", 10, 50, font, screen)
            # render_text(f"Yaw: {data[1]}", 10, 90, font, screen)

            # # Reactiva el test de profundidad
            # glEnable(GL_DEPTH_TEST)

            glPushMatrix()
            glRotatef(float(data[0]), 1, 0, 0)  # Rotar alrededor de los ejes x, y, z
            glRotatef(float(data[1]), 0, 1, 0)  # Rotar alrededor de los ejes x, y, z
            glRotatef(float(data[2]), 0, 0, 1)  # Rotar alrededor de los ejes x, y, z
            draw_prism()
            glPopMatrix()

            pygame.display.flip()
            pygame.time.wait(10)

        pygame.quit()
    except serial.SerialException as e:
        print(f"Error opening or communicating with serial port: {e}")

    except KeyboardInterrupt:
        print("Program interrupted by user.")

    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()
