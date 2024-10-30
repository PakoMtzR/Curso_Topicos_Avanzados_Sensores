
// Incluimos la libreria para la comunicacion i2c con el MPU6050
#include <Wire.h>

// Definimos registros necesarios para operar con el MPU6050
#define MPU6050_ADDRESS         0x68  
#define MPU6050_CONFIG          0x1A
#define MPU6050_GYRO_CONFIG     0x1B  
#define MPU6050_ACCEL_CONFIG    0x1C  
#define MPU6050_PWR_MGMT_1      0x6B

#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48

#define GYRO_SENSITIVITY  65.536 // Sensibilidad del giroscopio (LSB/°/s)

// Variables globales
int gyro_x=0, gyro_y=0, gyro_z=0;
double gyro_x_prom=0, gyro_y_prom=0, gyro_z_prom=0;
float wx0=0, wy0=0, wz0=0;  // Velocidad angular inicial
//float tx0=0, ty0=0, tz0=0;  // Posicion angular inicial
float wx=0, wy=0, wz=0;   // Velocidad angular
float tx=0, ty=0, tz=0;   // Posicion angular
//int accel_x=0, accel_y=0, accel_z=0;
//int temp=0;
int cont=0;
double h = 0.05;
//unsigned long t_0 = 0;
//unsigned long t_f = 0;
//unsigned long timer;

float wx_array[4] = {0,0,0,0};
float wy_array[4] = {0,0,0,0};
float wz_array[4] = {0,0,0,0};

// Funciones
void mpu6050_write(unsigned char config_address, unsigned char value);
void mpu6050_read();
void mpu6050_calibrate(int n);

void setup() {
  // Inicializamos comunucacion i2c
  Serial.begin(9600);
  Wire.begin();
  delay(50);

  // Configuracion del MPU6050
  mpu6050_write(MPU6050_CONFIG,0x06);       //(EXT_SYNC_SET=0, DLPF_CFG=0)
  mpu6050_write(MPU6050_GYRO_CONFIG,0x10);  //(XG_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=2 [1000°/s] )
  mpu6050_write(MPU6050_ACCEL_CONFIG,0x10); //(XA_ST=0, YA_ST=0, ZA_ST=0, FS_SEL=2 [8g] )
  mpu6050_write(MPU6050_PWR_MGMT_1,0x00);   //(DEVICE_RESET=0, SLEEP=0, CYCLE=0, TEMP_DIS=0, CLK_SEL=0 [Internal 8MHz oscillator] )
  delay(50);

  // Calibramos el MPU6050 con 1000 ciclos
  mpu6050_calibrate(2000);
}

void loop() {
  for(int i=1; i<4; i++) {
    mpu6050_read();

    wx = gyro_x - gyro_x_prom;
    wy = gyro_y - gyro_y_prom;
    wz = gyro_z - gyro_z_prom;

    if ((wx>-30) && (wx<30)) wx=0;
    if ((wy>-50) && (wy<50)) wy=0;
    if ((wz>-70) && (wz<70)) wz=0;

    wx = wx / GYRO_SENSITIVITY;
    wy = wy / GYRO_SENSITIVITY;
    wz = wz / GYRO_SENSITIVITY;

    wx_array[i] = wx;
    wy_array[i] = wy;
    wz_array[i] = wz;
  }
  //h = (micros() - timer) / 1000000.0;
  // Integracion numerica simpson 3/8 para calcular la posicion
  tx = tx + (3*h/8)*(wx_array[0] + 3*wx_array[1] + 3*wx_array[2] + wx_array[3]);
  ty = ty + (3*h/8)*(wy_array[0] + 3*wy_array[1] + 3*wy_array[2] + wy_array[3]);
  tz = tz + (3*h/8)*(wz_array[0] + 3*wz_array[1] + 3*wz_array[2] + wz_array[3]);

  wx_array[0] = wx_array[3];
  wy_array[0] = wy_array[3];
  wz_array[0] = wz_array[3];

  cont++;
  if(cont > 10) {
    cont=0;
    Serial.print(tx);
    Serial.print(", ");
    Serial.print(ty);
    Serial.print(", ");
    Serial.println(tz);
  }
  //timer = micros();
  delay(10);
}


void mpu6050_write(unsigned char config_address, unsigned char value) {
  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(config_address);
  Wire.write(value);
  Wire.endTransmission();
  delayMicroseconds(20);
}

void mpu6050_read() {
  unsigned char gyro_x_h=0, gyro_x_l=0, gyro_y_h=0, gyro_y_l=0, gyro_z_h=0, gyro_z_l=0;
  //unsigned char accel_x_h=0, accel_x_l=0, accel_y_h=0,accel_y_l=0, accel_z_h=0, accel_z_l=0;
  //unsigned char int temp_h=0, temp_l=0;
  
  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(GYRO_XOUT_H);
        
  Wire.requestFrom(MPU6050_ADDRESS, 6);
  gyro_x_h = Wire.read();
  gyro_x_l = Wire.read();
  gyro_y_h = Wire.read();
  gyro_y_l = Wire.read();
  gyro_z_h = Wire.read();
  gyro_z_l = Wire.read();
            
  Wire.endTransmission();
  gyro_x = (int)(gyro_x_h<<8 | gyro_x_l);
  gyro_y = (int)(gyro_y_h<<8 | gyro_y_l);
  gyro_z = (int)(gyro_z_h<<8 | gyro_z_l);
}

void mpu6050_calibrate(int n) {
  double sumX=0, sumY=0, sumZ=0;
  for(int i=0; i<n; i++) {
    mpu6050_read();
    sumX = sumX + gyro_x;
    sumY = sumY + gyro_y;
    sumZ = sumZ + gyro_z;
  }
  gyro_x_prom = sumX/n;
  gyro_y_prom = sumY/n;
  gyro_z_prom = sumZ/n;
}