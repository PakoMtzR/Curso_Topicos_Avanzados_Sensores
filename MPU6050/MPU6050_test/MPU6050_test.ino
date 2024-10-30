// MPU-6050 Acelerometro + Giroscopio

#include <Wire.h>

#include <Adafruit_GFX.h>   //Liberia para gráficos
#include <Adafruit_SSD1306.h>  //Liberia para Oleds monocromáticos basados ​​en controladores SSD1306
/*Se declara el pin de reset, este es requerido por el constructor de la librería SSD1306
 *para definir el tipo de OLED y la comunicación I2C en los (pines SDA, SCL)*/
#define OLED_RESET -1 
Adafruit_SSD1306 display(OLED_RESET);
/*Se define una condición para saber si en la librería esta definida la altura de 32 de lo contrario 
 *no dejara compilar el código hasta que se cambie la altura correcta desde la librería*/
#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta, cambie en la librería de Adafruit_SSD1306.h!");
#endif



//Registros de acuerdo a la hoja de especificaciones

#define MPU6050_AUX_VDDIO          0x01   // R/W
#define MPU6050_SMPLRT_DIV         0x19   // R/W
#define MPU6050_CONFIG             0x1A   // R/W
#define MPU6050_GYRO_CONFIG        0x1B   // R/W
#define MPU6050_ACCEL_CONFIG       0x1C   // R/W
#define MPU6050_FF_THR             0x1D   // R/W
#define MPU6050_FF_DUR             0x1E   // R/W
#define MPU6050_MOT_THR            0x1F   // R/W
#define MPU6050_MOT_DUR            0x20   // R/W
#define MPU6050_ZRMOT_THR          0x21   // R/W
#define MPU6050_ZRMOT_DUR          0x22   // R/W
#define MPU6050_FIFO_EN            0x23   // R/W
#define MPU6050_I2C_MST_CTRL       0x24   // R/W
#define MPU6050_I2C_SLV0_ADDR      0x25   // R/W
#define MPU6050_I2C_SLV0_REG       0x26   // R/W
#define MPU6050_I2C_SLV0_CTRL      0x27   // R/W
#define MPU6050_I2C_SLV1_ADDR      0x28   // R/W
#define MPU6050_I2C_SLV1_REG       0x29   // R/W
#define MPU6050_I2C_SLV1_CTRL      0x2A   // R/W
#define MPU6050_I2C_SLV2_ADDR      0x2B   // R/W
#define MPU6050_I2C_SLV2_REG       0x2C   // R/W
#define MPU6050_I2C_SLV2_CTRL      0x2D   // R/W
#define MPU6050_I2C_SLV3_ADDR      0x2E   // R/W
#define MPU6050_I2C_SLV3_REG       0x2F   // R/W
#define MPU6050_I2C_SLV3_CTRL      0x30   // R/W
#define MPU6050_I2C_SLV4_ADDR      0x31   // R/W
#define MPU6050_I2C_SLV4_REG       0x32   // R/W
#define MPU6050_I2C_SLV4_DO        0x33   // R/W
#define MPU6050_I2C_SLV4_CTRL      0x34   // R/W
#define MPU6050_I2C_SLV4_DI        0x35   // R  
#define MPU6050_I2C_MST_STATUS     0x36   // R
#define MPU6050_INT_PIN_CFG        0x37   // R/W
#define MPU6050_INT_ENABLE         0x38   // R/W
#define MPU6050_INT_STATUS         0x3A   // R  
#define MPU6050_ACCEL_XOUT_H       0x3B   // R  
#define MPU6050_ACCEL_XOUT_L       0x3C   // R  
#define MPU6050_ACCEL_YOUT_H       0x3D   // R  
#define MPU6050_ACCEL_YOUT_L       0x3E   // R  
#define MPU6050_ACCEL_ZOUT_H       0x3F   // R  
#define MPU6050_ACCEL_ZOUT_L       0x40   // R  
#define MPU6050_TEMP_OUT_H         0x41   // R  
#define MPU6050_TEMP_OUT_L         0x42   // R  
#define MPU6050_GYRO_XOUT_H        0x43   // R  
#define MPU6050_GYRO_XOUT_L        0x44   // R  
#define MPU6050_GYRO_YOUT_H        0x45   // R  
#define MPU6050_GYRO_YOUT_L        0x46   // R  
#define MPU6050_GYRO_ZOUT_H        0x47   // R  
#define MPU6050_GYRO_ZOUT_L        0x48   // R  
#define MPU6050_EXT_SENS_DATA_00   0x49   // R  
#define MPU6050_EXT_SENS_DATA_01   0x4A   // R  
#define MPU6050_EXT_SENS_DATA_02   0x4B   // R  
#define MPU6050_EXT_SENS_DATA_03   0x4C   // R  
#define MPU6050_EXT_SENS_DATA_04   0x4D   // R  
#define MPU6050_EXT_SENS_DATA_05   0x4E   // R  
#define MPU6050_EXT_SENS_DATA_06   0x4F   // R  
#define MPU6050_EXT_SENS_DATA_07   0x50   // R  
#define MPU6050_EXT_SENS_DATA_08   0x51   // R  
#define MPU6050_EXT_SENS_DATA_09   0x52   // R  
#define MPU6050_EXT_SENS_DATA_10   0x53   // R  
#define MPU6050_EXT_SENS_DATA_11   0x54   // R  
#define MPU6050_EXT_SENS_DATA_12   0x55   // R  
#define MPU6050_EXT_SENS_DATA_13   0x56   // R  
#define MPU6050_EXT_SENS_DATA_14   0x57   // R  
#define MPU6050_EXT_SENS_DATA_15   0x58   // R  
#define MPU6050_EXT_SENS_DATA_16   0x59   // R  
#define MPU6050_EXT_SENS_DATA_17   0x5A   // R  
#define MPU6050_EXT_SENS_DATA_18   0x5B   // R  
#define MPU6050_EXT_SENS_DATA_19   0x5C   // R  
#define MPU6050_EXT_SENS_DATA_20   0x5D   // R  
#define MPU6050_EXT_SENS_DATA_21   0x5E   // R  
#define MPU6050_EXT_SENS_DATA_22   0x5F   // R  
#define MPU6050_EXT_SENS_DATA_23   0x60   // R  
#define MPU6050_MOT_DETECT_STATUS  0x61   // R  
#define MPU6050_I2C_SLV0_DO        0x63   // R/W
#define MPU6050_I2C_SLV1_DO        0x64   // R/W
#define MPU6050_I2C_SLV2_DO        0x65   // R/W
#define MPU6050_I2C_SLV3_DO        0x66   // R/W
#define MPU6050_I2C_MST_DELAY_CTRL 0x67   // R/W
#define MPU6050_SIGNAL_PATH_RESET  0x68   // R/W
#define MPU6050_MOT_DETECT_CTRL    0x69   // R/W
#define MPU6050_USER_CTRL          0x6A   // R/W
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_PWR_MGMT_2         0x6C   // R/W
#define MPU6050_FIFO_COUNTH        0x72   // R/W
#define MPU6050_FIFO_COUNTL        0x73   // R/W
#define MPU6050_FIFO_R_W           0x74   // R/W
#define MPU6050_WHO_AM_I           0x75   // R
#define MPU6050_ADDRESS            0x68

 

void setup(void){
  Serial.begin(9600);
  Wire.begin();
  delay(50);
  MPU6050_configuracion(MPU6050_CONFIG,0x03);   //(EXT_SYNC_SET=0, DLPF_CFG=3)
  MPU6050_configuracion(MPU6050_GYRO_CONFIG,0x10); //(XG_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=2 [1000°/s] )
  MPU6050_configuracion(MPU6050_ACCEL_CONFIG,0x10); //(XA_ST=0, YA_ST=0, ZA_ST=0, FS_SEL=2 [8g] )
  MPU6050_configuracion(MPU6050_PWR_MGMT_1,0x00);  //(DEVICE_RESET=0, SLEEP=0,
 // CYCLE=0, TEMP_DIS=0, CLK_SEL=0 [Internal 8MHz oscillator] )
  delay(50);

   /* Se inicia la comunicación I2C en la direccion 0x3C para la pantalla oled 128x32*/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}
void loop(void){
  unsigned short int gyro_x_h=0, gyro_x_l=0, gyro_y_h=0, gyro_y_l=0,gyro_z_h=0, gyro_z_l=0;
  unsigned short int accel_x_h=0, accel_x_l=0, accel_y_h=0,accel_y_l=0, accel_z_h=0, accel_z_l=0;
  unsigned short int temp_h=0, temp_l=0;
  int gyro_x=0, gyro_y=0, gyro_z=0;
  int accel_x=0, accel_y=0, accel_z=0;
  int temp=0;

  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(MPU6050_ACCEL_XOUT_H);         
  Wire.requestFrom(MPU6050_ADDRESS, 14);
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
  accel_x=(((int)accel_x_h)<<8 )  |  ((int)accel_x_l); 
  accel_y=(((int)accel_y_h)<<8 )  |  ((int)accel_y_l); 
  accel_z=(((int)accel_z_h)<<8 )  |  ((int)accel_z_l); 
  temp=(((int)temp_h)<<8 )  |  ((int)temp_l); 
  gyro_x=(((int)gyro_x_h)<<8 )  |  ((int)gyro_x_l); 
  gyro_y=(((int)gyro_y_h)<<8 )  |  ((int)gyro_y_l); 
  gyro_z=(((int)gyro_z_h)<<8 )  |  ((int)gyro_z_l);

  Serial.print("Giroscopio x,y,z: ");
  Serial.print(gyro_x);
  Serial.print(",");
  Serial.print(gyro_y);
  Serial.print(",");
  Serial.println(gyro_z);
  Serial.print("Acelerometro x,y,z: ");
  Serial.print(accel_x);
  Serial.print(",");
  Serial.print(accel_y);
  Serial.print(",");
  Serial.println(accel_z);
  Serial.print("Temperatura: ");
  Serial.println((float)temp/340 + 36.53); //basado en la hoja de especificaciones
  Serial.println();
  mandar_oled(gyro_x, gyro_y, gyro_z, accel_x, accel_y, accel_z, temp);
  delay(50);
}

void MPU6050_configuracion(int r1, int r2){
  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(r1);
  Wire.write(r2);
  Wire.endTransmission();
  delayMicroseconds(20);
}

void mandar_oled(int gyro_x, int gyro_y, int gyro_z, int accel_x, int accel_y, int accel_z, int temp) { 
  display.clearDisplay(); //Borra el buffer
  display.setTextSize(1); //Establece el tamaño de fuente, admite tamaños de 1 a 8
  display.setTextColor(WHITE); //Establece el color 
  display.setCursor(0,5); //Establecer las coordenadas para mostrar la posición del texto
  display.print("Acce:");
  display.print(accel_x);
  display.print(", ");
  display.print(accel_y);
  display.print(", ");
  display.println(accel_z);

  display.print("Gryo:");
  display.print(gyro_x);
  display.print(", ");
  display.print(gyro_y);
  display.print(", ");
  display.println(gyro_z);

  display.print("Temp:");
  display.println((float)temp/340 + 36.53);

  display.display(); //Muestra el texto 
}

