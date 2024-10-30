import serial
import time

# Configura el puerto serial
arduino_port = "COM6"  # Reemplaza con el puerto correcto
baud_rate = 9600
timeout = 1

# Inicializa la comunicación serial
ser = serial.Serial(arduino_port, baud_rate)

time.sleep(2)  # Espera un poco para que la conexión se establezca

# Envía datos al Arduino
ser.write(b'Hello from Python!\n')

# Lee datos del Arduino
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()
        print(f"Received from Arduino: {line}")

# Cierra la conexión serial (opcional)
ser.close()