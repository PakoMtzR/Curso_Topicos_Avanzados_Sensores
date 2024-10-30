import machine
import utime

# Definimos el I2C
sda = machine.Pin(0)
scl = machine.Pin(1)
i2c = machine.I2C(0, scl=scl, sda=sda, freq=400000)

# Direcciones de los registros del MPU6050
address = 0x68
GYRO_XOUT_H = 0x43

# Configuraciones
CONFIG = 0x1A
GYRO_CONFIG = 0x1B
ACCEL_CONFIG = 0x1C
PWR_MGMT_1 = 0x6B

i2c.writeto_mem(address, ACCEL_CONFIG, b'\x10') # ±8g(0X10) ±2g(0X00)
i2c.writeto_mem(address, PWR_MGMT_1, b'\x00')
i2c.writeto_mem(address, GYRO_CONFIG, b'\x00')
i2c.writeto_mem(address, CONFIG, b'\x06') # Filtro 5Hz(0X06)
utime.sleep(1)

# Función para leer el giroscopio
def leerGiroscopio():
    gyro_x_data = i2c.readfrom_mem(address, GYRO_XOUT_H, 6)
    gyro_x = (gyro_x_data[0] << 8 | gyro_x_data[1]) if gyro_x_data[0] < 128 else ((gyro_x_data[0] << 8 | gyro_x_data[1]) - 65536)
    gyro_y = (gyro_x_data[2] << 8 | gyro_x_data[3]) if gyro_x_data[2] < 128 else ((gyro_x_data[2] << 8 | gyro_x_data[3]) - 65536)
    gyro_z = (gyro_x_data[4] << 8 | gyro_x_data[5]) if gyro_x_data[4] < 128 else ((gyro_x_data[4] << 8 | gyro_x_data[5]) - 65536)
    return gyro_x, gyro_y, gyro_z

# Función para calibrar el giroscopio
def calibrar():
    sumX = sumY = sumZ = 0
    for _ in range(1000):
        gyro_x, gyro_y, gyro_z = leerGiroscopio()
        sumX += gyro_x
        sumY += gyro_y
        sumZ += gyro_z
        utime.sleep_ms(1)
    return sumX / 1000, sumY / 1000, sumZ / 1000

# Calibramos el giroscopio
sumX, sumY, sumZ = calibrar()
coordx0 = coordy0 = coordz0 = cont = 0
wx0 = wy0 = wz0 = 0

# Bucle principal
while True:
    gyro_x, gyro_y, gyro_z = leerGiroscopio()
    
    # Cálculamos el error
    wx = gyro_x - sumX
    wy = gyro_y - sumY
    wz = gyro_z - sumZ
    
    # Error en x
    if -30 < wx < 30:
        wx = 0
    # Error en y    
    if -50 < wy < 50:
        wy = 0
    # Error en z    
    if -70 < wz < 70:
        wz = 0
    
    # Ajustamos los errores
    wx = wx * 150 / 65536
    wy = wy * 155 / 65536
    wz = wz * 155 / 65536
    
    # Actualizamos las posiciones
    coordx = coordx0 + (0.05 * (3) * (wx0 + 3 * (wx) + 3 * (wx) + 3 * (wx))/8)
    coordy = coordy0 + (0.05 * (3) * (wy0 + 3 * (wy) + 3 * (wy) + 3 * (wy))/8)
    coordz = coordz0 + (0.05 * (3) * (wz0 + 3 * (wz) + 3 * (wz) + 3 * (wz))/8)
    
    # Almacenamos los valores obtenidos anteriormente
    w0=wx;
    w0=wy;
    w0=wz; 
    coordx0 = coordx
    coordy0 = coordy
    coordz0 = coordz
    cont += 1
    
    # Imprimir valores cada 10 iteraciones
    if cont > 10:
        print("coordx: {:.2f}, coordy: {:.2f}, coordz: {:.2f}".format(coordx, coordy, coordz)) #Para mostrar los 3 ejes
        #print("coordx: {:.2f}, coordz: {:.2f}".format(coordx, coordz)) #Para mostrar 1 plano
        cont = 0
    
    utime.sleep_ms(100)