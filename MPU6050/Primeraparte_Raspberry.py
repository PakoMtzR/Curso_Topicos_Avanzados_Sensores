from machine import I2C, Pin
import time
import math

# Configuramos la conexion i2c
i2c = I2C(0, scl=Pin(1), sda=Pin(0), freq=400000)

# Registros del MPU6050
ADDRESS = 0x68
CONFIG = 0x1A
ACCEL_CONFIG = 0x1C
ACCEL_XOUT_H = 0x3B
GYRO_CONFIG = 0x1B
PWR_MGMT_1 = 0x6B

# AFS_SEL valores de configuracion del acelerometro
# 0x00 => +-2g
# 0x08 => +-4g
# 0x10 => +-8g
# 0x18 => +-16g
AFS_SEL = [b'\x00', b'\x08', b'\x10', b'\x18']
acce_config = AFS_SEL[2]
offset = {b'\x00':[-450,0,1500], b'\x08':[-220,0,790], b'\x10':[-100,0,400], b'\x18':[-60,0,200]}

DLPF_CFG = [b'\x00', b'\x01', b'\x02', b'\x03', b'\x04', b'\x05', b'\x06']

# Escribimos en los registros del MPU6050
i2c.writeto_mem(ADDRESS, CONFIG, DLPF_CFG[6])		# Configuramos el filtro
i2c.writeto_mem(ADDRESS, ACCEL_CONFIG, acce_config)	# Configura el rango de escala del acelerometro +-8g
i2c.writeto_mem(ADDRESS, PWR_MGMT_1, b'\x00')		# 
time.sleep(1)

acce_data = []
#Esta parte pertenece a la conversion a grados de la parte 2 de la practica
# def get_deg(x,y):
#     angulo_grados = math.degrees(math.atan2(x, y))
#     if angulo_grados < 0:
#         angulo_grados += 360
#     return round(angulo_grados, 2)

while True:
    acce_data.clear()
    for i in range(0,6,2):
        high = i2c.readfrom_mem(ADDRESS, ACCEL_XOUT_H + i, 1)[0]
        low = i2c.readfrom_mem(ADDRESS, ACCEL_XOUT_H + i + 1, 1)[0]
        value = (high << 8) | low
        # 0x8000 = b1000 0000 0000 0000
        if value & 0x8000:
            value = -((value ^ 0xFFFF) + 1) # Extensión de signo
        acce_data.append(value)
    
    acce_data[0] += offset[acce_config][0]
    acce_data[1] += offset[acce_config][1]
    acce_data[2] += offset[acce_config][2]    
    
    print(f"X:{acce_data[0]}, Y:{acce_data[1]}, Z:{acce_data[2]}") #angXZ={get_deg(acce_data[0], acce_data[2])}°, angYZ={get_deg(acce_data[1], acce_data[2])}°
    time.sleep(1)
