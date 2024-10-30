#include <Wire.h> //Para comunicación I2C
#include <math.h> //Para funciones trigonométricas.
#define LSM303DLH (0x3C >> 1) //Dirección del dispositivo
//REGISTROS A USAR DE MAGNETOMETRO
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
double xm,ym,zm; //Almacenan lectura completa de los 3 ejes 
int norte; //Grados a los que se encuentra el norte
void setup() 
{
Serial.begin(9600); //Inicia comunicación serie con PC a 9600 baudios 
Wire.begin(); //Inicia comunicación I2C con dispositivo 

//Configuración del Magnetometro

write_register(CRA_REG_M,0x18); //inicia escritura en dirección del registro CRA
write_register(CRB_REG_M,0b01100000);
write_register(MR_REG_M,0x00); 
}
void loop() {
Wire.beginTransmission(LSM303DLH); //Comunicarse con sensor - Escritura 
Wire.write(OUT_X_H_M); //Leer a partir de x alta 
Wire.endTransmission(); //Fin comunicación con sensor - Escritura 
delay(1);
Wire.requestFrom(LSM303DLH,6); //Leer los 6 valores de los 3 ejes,
  xhm=Wire.read();// 2 datos por eje (alto y bajo) 
  xlm=Wire.read();
  zhm=Wire.read();
  zlm=Wire.read();
  yhm=Wire.read();
  ylm=Wire.read();
xm=(long)(xhm<<8)|xlm; //Unión de parte alta y baja de las lecturas 
ym=(long)(yhm<<8)|ylm;
ym=ym+50;
xm=xm+55;
zm=(long)(zhm<<8)|zlm;
norte=atan2(ym,xm)*(180/3.14159); //Angulo en grados dirección del Norte
  if (norte<0){ //Ajuste
    norte +=360;
}

  Serial.print("x: ");
  Serial.print(xm);
  Serial.print("  y: ");
  Serial.print(ym);
  Serial.print("  z: ");
  Serial.println(zm);
  Serial.println(norte); //Envió de datos serie a la PC

  delay(1000);
}

void write_register(byte address, byte value){
  Wire.beginTransmission(LSM303DLH);
  Wire.write(address); 
  Wire.write(value); 
  Wire.endTransmission();
}
