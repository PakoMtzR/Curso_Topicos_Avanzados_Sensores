#include <Wire.h>
#include <math.h>

// Direcciones y/o registros del MPU6050
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

void mpu6050_write(unsigned char config_address, unsigned char value);
void mpu6050_read();
void mpu6050_calibrate_gyro(int n);
void updateQuaternion(float gx, float gy, float gz, float dt);
void calculateEulerAngles(float q[]);

// Variables de sensores
double gyro_x_prom=0, gyro_y_prom=0, gyro_z_prom=0;
int gyro_x=0, gyro_y=0, gyro_z=0;
int accel_x=0, accel_y=0, accel_z=0;
int temp=0;

// Variables de cuaterniones
float q[4] = {1.0, 0.0, 0.0, 0.0}; // Cuaterniones iniciales
float gyroScale = 32.8; // Escala del giroscopio para +/- 1000 grados/seg
unsigned long lastUpdate = 0; // Para el cálculo del tiempo delta

void setup() {
  // Inicializamos comunicacion i2c
  Serial.begin(9600);
  Wire.begin();
  delay(50);

  // Configuracion del MPU6050
  mpu6050_write(MPU6050_CONFIG, 0x06);       // (EXT_SYNC_SET=0, DLPF_CFG=0)
  mpu6050_write(MPU6050_GYRO_CONFIG, 0x10);  // (XG_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=2 [1000°/s])
  mpu6050_write(MPU6050_ACCEL_CONFIG, 0x10); // (XA_ST=0, YA_ST=0, ZA_ST=0, FS_SEL=2 [8g])
  mpu6050_write(MPU6050_PWR_MGMT_1, 0x00);   // (DEVICE_RESET=0, SLEEP=0, CYCLE=0, TEMP_DIS=0, CLK_SEL=0 [Internal 8MHz oscillator])
  delay(50);

  // Calibramos el giroscopio MPU6050 con 2000 ciclos
  mpu6050_calibrate_gyro(500);
  lastUpdate = millis();
}

void loop() {
  mpu6050_read();

  // Calcula el tiempo transcurrido (dt) en segundos
  unsigned long currentTime = millis();
  float dt = (currentTime - lastUpdate) / 1000.0;
  lastUpdate = currentTime;

  // Convertir los datos de giroscopio a radianes/segundo
  float gx = (gyro_x - gyro_x_prom) / gyroScale * DEG_TO_RAD;
  float gy = (gyro_y - gyro_y_prom) / gyroScale * DEG_TO_RAD;
  float gz = (gyro_z - gyro_z_prom) / gyroScale * DEG_TO_RAD;

  // Actualizar cuaterniones
  updateQuaternion(gx, gy, gz, dt);

  // Calcular ángulos de Euler
  calculateEulerAngles(q);

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
  // Declaracion de variables
  unsigned short int gyro_x_h=0, gyro_x_l=0, gyro_y_h=0, gyro_y_l=0, gyro_z_h=0, gyro_z_l=0;
  unsigned short int accel_x_h=0, accel_x_l=0, accel_y_h=0, accel_y_l=0, accel_z_h=0, accel_z_l=0;
  unsigned short int temp_h=0, temp_l=0;

  // Lectura de datos del MPU6050
  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(ACCEL_XOUT_H);         
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDRESS, 14, true);
  accel_x_h = Wire.read();
  accel_x_l = Wire.read();
  accel_y_h = Wire.read();
  accel_y_l = Wire.read();
  accel_z_h = Wire.read();
  accel_z_l = Wire.read();
  temp_h = Wire.read();
  temp_l = Wire.read();
  gyro_x_h = Wire.read();
  gyro_x_l = Wire.read();
  gyro_y_h = Wire.read();
  gyro_y_l = Wire.read();
  gyro_z_h = Wire.read();
  gyro_z_l = Wire.read();
  Wire.endTransmission();

  // Combinar lecturas de alto y bajo byte
  accel_x = (int16_t)(accel_x_h << 8 | accel_x_l); 
  accel_y = (int16_t)(accel_y_h << 8 | accel_y_l); 
  accel_z = (int16_t)(accel_z_h << 8 | accel_z_l);

  gyro_x = (int16_t)(gyro_x_h << 8 | gyro_x_l);
  gyro_y = (int16_t)(gyro_y_h << 8 | gyro_y_l);
  gyro_z = (int16_t)(gyro_z_h << 8 | gyro_z_l);
}

void mpu6050_calibrate_gyro(int n) {
  double sumX = 0, sumY = 0, sumZ = 0;
  for(int i = 0; i < n; i++) {
    mpu6050_read();
    sumX += gyro_x;
    sumY += gyro_y;
    sumZ += gyro_z;
    delay(3); // Pequeño retardo para evitar sobrecarga del MPU6050
  }
  gyro_x_prom = sumX / n;
  gyro_y_prom = sumY / n;
  gyro_z_prom = sumZ / n;
}

void updateQuaternion(float gx, float gy, float gz, float dt) {
  // Cuaterniones de velocidad angular
  float q0_dot = 0.5 * (-q[1] * gx - q[2] * gy - q[3] * gz);
  float q1_dot = 0.5 * (q[0] * gx + q[2] * gz - q[3] * gy);
  float q2_dot = 0.5 * (q[0] * gy - q[1] * gz + q[3] * gx);
  float q3_dot = 0.5 * (q[0] * gz + q[1] * gy - q[2] * gx);
  
  // Integrar para obtener los nuevos cuaterniones
  q[0] += q0_dot * dt;
  q[1] += q1_dot * dt;
  q[2] += q2_dot * dt;
  q[3] += q3_dot * dt;
  
  // Normalizar los cuaterniones
  float norm = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
  q[0] /= norm;
  q[1] /= norm;
  q[2] /= norm;
  q[3] /= norm;
}

void calculateEulerAngles(float q[]) {
  // Convertir cuaterniones a ángulos de Euler
  float roll = atan2(2.0 * (q[0] * q[1] + q[2] * q[3]), 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2])) * RAD_TO_DEG;
  float pitch = asin(2.0 * (q[0] * q[2] - q[3] * q[1])) * RAD_TO_DEG;
  float yaw = atan2(2.0 * (q[0] * q[3] + q[1] * q[2]), 1.0 - 2.0 * (q[2] * q[2] + q[3] * q[3])) * RAD_TO_DEG;

  // Imprimir ángulos de Euler
  // Serial.print("Roll: ");
  // Serial.print(roll);
  // Serial.print(" Pitch: ");
  // Serial.print(pitch);
  // Serial.print(" Yaw: ");
  // Serial.println(yaw);


  Serial.print(pitch);
  Serial.print(",");
  Serial.print(yaw);
  Serial.print(",");
  Serial.println(roll);
}

