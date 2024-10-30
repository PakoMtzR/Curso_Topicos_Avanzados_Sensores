/*
 * brujula.c
 *
 * Created: 10/11/2023 8:26:08
 * Author: Jhace
 */
#define LSM303DLH (0x3C >> 1) // Dirección del dispositivo // Regla de protocolo

//REGISTROS A USAR DE MAGNETOMETRO
#define W_DATA 0x3C
#define R_DATA 0x3D        
#define CRA_REG_M 0x00 //Configuraciones.
#define CRB_REG_M 0x01 
#define MR_REG_M 0x02 //parte alta y baja.
#define OUT_X_H_M 0x03//Lectura de la parte alta del eje x
#define OUT_X_L_M 0x04//Lectura de la parte baja del eje x
#define OUT_Z_H_M 0x05//Lectura de la parte alta del eje z
#define OUT_Z_L_M 0x06//Lectura de la parte baja del eje z
#define OUT_Y_H_M 0x07//Lectura de la parte alta del eje y
#define OUT_Y_L_M 0x08//Lectura de la parte baja del eje y

#include <io.h>
#include <stdio.h>
#include <delay.h>
#include <stdint.h>
#include <math.h>
#include "I2C.c"
#include "USART.h"
void LSM303_Write(unsigned char add, unsigned char data) // Para escribir en I2C en el sensor
{
   I2C_start();
   I2C_write(W_DATA); // or write o como ya estaba (con D0 y D1)
   I2C_write(add);
   I2C_write(data);
   I2C_stop();
}
int LSM303_Read(unsigned char add)
{
    int retval;
    I2C_start();
    I2C_write(W_DATA);
    I2C_write(add);
    I2C_start();
    I2C_write(R_DATA);
    retval = I2C_read_nack();
    I2C_stop();
    return retval;
}
void LSM303_Init(void)
{
   LSM303_Write(CRA_REG_M,0x18);
   LSM303_Write(MR_REG_M,0x00);
}
float LSM303_Read_Mx(void)
{
    return (float)((int)LSM303_Read(OUT_X_H_M) << 8 | LSM303_Read(OUT_X_L_M));
}
float LSM303_Read_My(void)
{
    return (float)((int)LSM303_Read(OUT_Y_H_M) << 8 | LSM303_Read(OUT_Y_L_M));
}
float LSM303_Read_Mz(void)
{
    return (float)((int)LSM303_Read(OUT_Z_H_M) << 8 | LSM303_Read(OUT_Z_L_M));
}

double xm,ym,zm; //Almacenan lectura completa de los 3 ejes para concatenar las anteriores
float norte; //Grados a los que se encuentra el norte
char data[10],data2[4];

void main(void)
{
I2C_init();
USART_init();
LSM303_Init();
while (1)
    {
    
    xm = LSM303_Read_Mx();                 // Lectura del eje x del acelerometro
    ym = LSM303_Read_My();                 // Lectura del eje y del acelerometro
    zm = LSM303_Read_Mz(); 
    ym=ym+50;  // Calibración con brújula analógica
    xm=xm+55; 
    
    /*if (ym>360){
    ym=0;}
    else if (xm>360){
    xm=0;} 
    else if (zm>360){
    zm=0;}*/ 
    
    norte=atan2((float)ym,(float)xm)*(180/3.14159); //Angulo en grados dirección del Norte // Se convierte a grados
    if (norte<0){ //Ajuste 
    norte +=360;
    }  
    sprintf(data,"x: %.2f,y: %.2f,z: %.2f\r",xm,ym,zm); // %c es para ascii (caracteres) // %d es para int
    USART_txstr(data);
    sprintf(data2,"%.2f\r",norte);
    USART_txstr(data2);
    delay_ms(200);
    }
}
