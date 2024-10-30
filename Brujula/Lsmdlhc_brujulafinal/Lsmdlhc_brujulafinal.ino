#include <Wire.h>
#include<math.h>

#define LSM303_ADDRESS_ACCEL (0x32 >> 1) // Dirección del acelerómetro LSM303
#define LSM303_ADDRESS_MAG (0x3C >> 1)   // Dirección del magnetómetro LSM303

//REGISTROS A USAR DEL ACELEROMETRO
#define LSM303_REGISTER_ACCEL_CTRL_REG1_A 0x20 // Registro de control 1 del acelerómetro
#define LSM303_REGISTER_ACCEL_OUT_X_L_A 0x28   // Registro de salida del eje X del acelerómetro (bajo)

#define CRA_REG_M 0x00 //Configuraciones.
#define CRB_REG_M 0x01 
#define MR_REG_M 0x02 //parte alta y baja.
#define OUT_X_H_M 0x03//Lectura de la parte alta del eje x
#define OUT_X_L_M 0x04//Lectura de la parte baja del eje x
#define OUT_Z_H_M 0x05//Lectura de la parte alta del eje z
#define OUT_Z_L_M 0x06//Lectura de la parte baja del eje z
#define OUT_Y_H_M 0x07//Lectura de la parte alta del eje y
#define OUT_Y_L_M 0x08//Lectura de la parte baja del eje y


int xhm,xlm,yhm,ylm,zhm,zlm; //Almacenan lecturas altas y bajas de los ejes 
double accel_x,accel_y,accel_z, mag_x, mag_y, mag_z; //Almacenan lectura completa de los 3 ejes 
int norte; //Grados a los que se encuentra el norte

void setup() {
  Serial.begin(9600);
  Wire.begin();

  //Configuración del Magnetometro
  write_register(CRA_REG_M,0x18); //inicia escritura en dirección del registro CRA
  write_register(CRB_REG_M,0b01100000);
  write_register(MR_REG_M,0x00);

  // Configurar el acelerómetro
  Wire.beginTransmission(LSM303_ADDRESS_ACCEL);
  Wire.write(LSM303_REGISTER_ACCEL_CTRL_REG1_A);
  Wire.write(0x27); // Habilitar el acelerómetro
  Wire.endTransmission();

//  // Configurar el magnetómetro (si es necesario)
//  Wire.beginTransmission(LSM303_ADDRESS_MAG);
//  Wire.write(0x00); // Registro de configuración A del magnetómetro
//  Wire.write(0x14); // Configurar las tasas de datos y modo de funcionamiento
//  Wire.endTransmission();

  delay(500);
}

void loop() {

  // Leer el acelerómetro
  Wire.beginTransmission(LSM303_ADDRESS_ACCEL);
  Wire.write(LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80); // Iniciar lectura desde el registro más bajo con auto-incremento
  Wire.endTransmission();
  Wire.requestFrom(LSM303_ADDRESS_ACCEL, 6);
  accel_x = (Wire.read() | (Wire.read() << 8));
  accel_y = (Wire.read() | (Wire.read() << 8));
  accel_z = (Wire.read() | (Wire.read() << 8));

  // Leer el magnetómetro
  Wire.beginTransmission(LSM303_ADDRESS_MAG); //Comunicarse con sensor - Escritura 
  Wire.write(OUT_X_H_M); //Leer a partir de x alta 
  Wire.endTransmission(); //Fin comunicación con sensor - Escritura 
  delay(1);
  Wire.requestFrom(LSM303_ADDRESS_MAG,6); //Leer los 6 valores de los 3 ejes,
  xhm=Wire.read();// 2 datos por eje (alto y bajo) 
  xlm=Wire.read();
  zhm=Wire.read();
  zlm=Wire.read();
  yhm=Wire.read();
  ylm=Wire.read();

  mag_x=(long)(xhm<<8)|xlm; //Unión de parte alta y baja de las lecturas 
  mag_y=(long)(yhm<<8)|ylm;
  mag_y=mag_y+50;
  mag_x=mag_x+55;
  mag_z=(long)(zhm<<8)|zlm;

  norte=atan2(mag_y,mag_x)*(180/3.14159); //Angulo en grados dirección del Norte
  if (norte<0){ //Ajuste
    norte +=360;

  }
  Serial.print(accel_x);
  Serial.print(",");
  Serial.print(accel_y);
  Serial.print(",");
  Serial.print(accel_z);
  Serial.print(",");
  Serial.print(mag_x);
  Serial.print(",");
  Serial.print(mag_y);
  Serial.print(",");
  Serial.println(mag_z);

  // Enviar los datos por el puerto serial
  // Serial.print("Acelerómetro: ");
  // Serial.print(accel_x);
  // Serial.print(",");
  // Serial.print(accel_y);
  // Serial.print(",");
  // Serial.println(accel_z);

  // Serial.print("Magnetómetro: ");
  // Serial.print(mag_x);
  // Serial.print(",");
  // Serial.print(mag_y);
  // Serial.print(",");
  // Serial.println(mag_z);
  
  // Serial.println(norte); //Envió de datos serie a la PC

  

  delay(1000);
}
void write_register(byte address, byte value){
  Wire.beginTransmission(LSM303_ADDRESS_MAG);
  Wire.write(address); 
  Wire.write(value); 
  Wire.endTransmission();
}

