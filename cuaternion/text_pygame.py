import pygame
import sys

# Inicializa Pygame
pygame.init()

# Configuración de la pantalla
width, height = 800, 600
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("Ejemplo de Texto en Pygame")

# Color de fondo
background_color = (30, 30, 30)

# Color del texto
text_color = (255, 255, 255)

# Fuente y tamaño del texto
font = pygame.font.SysFont("Arial", 36)

# Texto a mostrar
text = "Hola, Mundo!"

# Ciclo principal del juego
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Rellena la pantalla con el color de fondo
    screen.fill(background_color)

    # Renderiza el texto en una superficie
    text_surface = font.render(text, True, text_color)

    # Obtiene el rectángulo del texto
    text_rect = text_surface.get_rect()

    # Centra el texto en la pantalla
    text_rect.center = (width // 2, height // 2)

    # Dibuja el texto en la pantalla
    screen.blit(text_surface, text_rect)

    # Actualiza la pantalla
    pygame.display.flip()

# Cierra Pygame
pygame.quit()
sys.exit()
