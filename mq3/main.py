# ********************************Sensor de Alcohol**********************************
# *                                                                                 *
# * El MQ-3 es un sensor que mide la concentración en mg/L de alcohol en el aire.   *
# *                                                                                 *
# * El siguiente programa realiza la lectura del sensor de alcohol MQ-3 y en base a *
# * este dato calcula la concentración en mg/L en el aire.                          *
# ***********************************************************************************

import machine
import utime
import math
from ssd1306 import SSD1306_I2C


WIDTH = 128  # oled display width
HEIGHT = 32  # oled display height

# Init I2C using pins GP8 & GP9 (default I2C0 pins)
i2c = machine.I2C(0, scl=machine.Pin(1), sda=machine.Pin(0), freq=200000)
print("I2C Address      : " + hex(i2c.scan()[0]).upper())  # Display device address
print("I2C Configuration: " + str(i2c))  # Display I2C config

oled = SSD1306_I2C(WIDTH, HEIGHT, i2c)  # Init oled display

# Se declaran las variables empleadas
div_voltaje = 0.0
rs_ro = 0.0
alcohol = 0.0
vout_sensor = 0.0

factor = 3.3 / 65535

# Configuración de pines
adc0 = machine.ADC(26)  # A0 conectado a GPIO26 (pin 31)
adc1 = machine.ADC(27)  # A1 conectado a GPIO27 (pin 32)

while True:
    vout_sensor = adc0.read_u16() * factor  # Lectura del sensor
    div_voltaje = adc1.read_u16() * factor

    # Cálculo del cociente Rs/Ro
    if vout_sensor != 0:
        rs_ro = (3.3 / vout_sensor) - 1
    else:
        rs_ro = 0  # Evitar división por cero

    # Cálculo de la concentración de alcohol
    alcohol = 0.354 * math.pow(rs_ro, -1.518)

    # Se imprime en el monitor serie la concentración de alcohol en el aire
    print(f"Concentracion de alcohol: {alcohol:.2f} mg/L")
    print(f"Voltaje del sensor: {vout_sensor:.2f} v\n")

    # Imprime en la pantalla oled
    oled.fill(0)
    oled.text("Alcohol: {:.2f} mg/L".format(alcohol), 0, 0)
    #oled.text("Voltaje: {:.2f} V".format(vout_sensor), 0, 10)
    oled.show()

    # Retardo por estabilidad (1 segundo)
    utime.sleep(1)  