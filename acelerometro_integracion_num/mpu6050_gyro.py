# Importamos las librerias necesarias
import machine
from utime import sleep_ms

# Definimos los pines de conexion del i2c
SDA = machine.Pin(0)
SCL = machine.Pin(1)

# Inicializamos la comunicacion i2c
i2c = machine.I2C(0, scl=SCL, sda=SDA, freq=400000)

# Registros del MPU6050
MPU6050_ADDRESS = 		0x68
MPU6050_CONFIG = 		0x1A
MPU6050_GYRO_CONFIG = 	0x1B
MPU6050_ACCEL_CONFIG = 	0x1C
MPU6050_PWR_MGMT_1 = 	0x6B

GYRO_XOUT_H = 0x43	# Registro de datos

GYRO_SENSITIVITY = 150/65536	# Factor para la sensibilidad del giroscopio

def mpu6050_config():
    i2c.writeto_mem(MPU6050_ADDRESS, MPU6050_CONFIG, b'\x06') 		# Filtro 5Hz gyro
    i2c.writeto_mem(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, b'\x00')	# ±250°/s
    i2c.writeto_mem(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, b'\x10') # ±8g
    i2c.writeto_mem(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, b'\x00')	
    sleep_ms(500)

def mpu6050_read():
    # Leemos los datos del giroscopio
    gyro_x_data = i2c.readfrom_mem(MPU6050_ADDRESS, GYRO_XOUT_H, 6)
    # Guardamos y verificamos si existen valores negativos
    gyro_x = (gyro_x_data[0] << 8 | gyro_x_data[1]) if gyro_x_data[0] < 128 else ((gyro_x_data[0] << 8 | gyro_x_data[1]) - 65536)
    gyro_y = (gyro_x_data[2] << 8 | gyro_x_data[3]) if gyro_x_data[2] < 128 else ((gyro_x_data[2] << 8 | gyro_x_data[3]) - 65536)
    gyro_z = (gyro_x_data[4] << 8 | gyro_x_data[5]) if gyro_x_data[4] < 128 else ((gyro_x_data[4] << 8 | gyro_x_data[5]) - 65536)
    return gyro_x, gyro_y, gyro_z

def mpu6050_calibrate(n):
    # Inicializamos las variables de las sumas
    sumX = sumY = sumZ = 0
    # sumamos las medidas de cada eje n veces
    for j in range(n):
        gyro_x, gyro_y, gyro_z = mpu6050_read()
        sumX += gyro_x
        sumY += gyro_y
        sumZ += gyro_z
        sleep_ms(1)
    # Calculamos el promedio    
    gyro_x_prom = sumX / n
    gyro_y_prom = sumY / n
    gyro_z_prom = sumZ / n
    # Retornamos valor promedio de cada eje
    return gyro_x_prom, gyro_y_prom, gyro_z_prom

# ========================================================================================================
# 									Inicio del programa
# ========================================================================================================
mpu6050_config()												# Configuracion del mpu6050
gyro_x_prom, gyro_y_prom, gyro_z_prom = mpu6050_calibrate(1000)	# Calibramos el giroscopio

# Inicializacion de variables globales
wx0 = wy0 = wz0 = 0		# Velocidad angular
tx0 = ty0 = tz0 = 0		# Posicion angular
h = 0.05

while True:
    for i in range(10):
        # Leemos datos crudos del giroscopio mpu6050 
        gyro_x, gyro_y, gyro_z = mpu6050_read()
        
        # Corregimos el error
        wx = gyro_x - gyro_x_prom
        wy = gyro_y - gyro_y_prom
        wz = gyro_z - gyro_z_prom
        
        if (-30<wx and wx<30): wx = 0 
        if (-30<wy and wy<30): wy = 0 
        if (-30<wz and wz<30): wz = 0 
        
        # Ajustamos el error
        wx = wx * GYRO_SENSITIVITY
        wy = wy * GYRO_SENSITIVITY
        wz = wz * GYRO_SENSITIVITY
        
        # Calculamos el angulo mediante integracion numerica simpson 3/8
        tx = tx0 + (h*(3/8)*(wx0 + 3*wx + 3*wx + 3*wx))
        ty = ty0 + (h*(3/8)*(wy0 + 3*wy + 3*wy + 3*wy))
        tz = tz0 + (h*(3/8)*(wz0 + 3*wz + 3*wz + 3*wz))
        
        # Actualizamos los datos
        w0 = wx; w0 = wy; w0 = wz 
        tx0 = tx; ty0 = ty; tz0 = tz
        
        sleep_ms(50)
        
    # Mostramos posiciones
    print(f"tx: {tx:.2f}, ty: {ty:.2f}, tz: {tz:.2f}")
