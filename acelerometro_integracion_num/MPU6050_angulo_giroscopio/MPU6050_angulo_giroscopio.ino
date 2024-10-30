// MPU-6050 Acelerometro + Giroscopio

#include <Wire.h>

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

float wx0=0,wy0=0,wz0=0;
float tx0=0,ty0=0,tz0=0;
int gyro_x=0, gyro_y=0, gyro_z=0;
double sumX=0,sumY=0,sumZ=0;
 
int cont=0;

void setup(void){
  Serial.begin(9600);
  Wire.begin();
  delay(50);
  MPU6050_configuracion(MPU6050_CONFIG,0x03);   //(EXT_SYNC_SET=0, DLPF_CFG=0)
  MPU6050_configuracion(MPU6050_GYRO_CONFIG,0x10); //(XG_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=2 [1000°/s] )
  MPU6050_configuracion(MPU6050_ACCEL_CONFIG,0x10); //(XA_ST=0, YA_ST=0, ZA_ST=0, FS_SEL=2 [8g] )
  MPU6050_configuracion(MPU6050_PWR_MGMT_1,0x00);  //(DEVICE_RESET=0, SLEEP=0, CYCLE=0, TEMP_DIS=0, CLK_SEL=0 [Internal 8MHz oscillator] )
  delay(50);
  calibrar();
}

void loop(void){
  float wx=0,wy=0, wz=0, tx=0, ty=0, tz=0; 
  leerGiroscopio();
  
  
  Serial.print(gyro_x);
  Serial.print(",");
  Serial.print(gyro_y);
  Serial.print(",");
  Serial.println(gyro_z);
  

  wx=((float)gyro_x-sumX)*2000/(65536);
  wy=((float)gyro_y-sumY)*2000/(65536); 
  wz=((float)gyro_z-sumZ)*2000/(65536); 

  tx=tx0+(0.05*(wx0+wx)/2);
  ty=ty0+(0.05*(wy0+wy)/2);
  tz=tz0+(0.05*(wz0+wz)/2);
  
  wx0=wx;
  wy0=wy;
  wz0=wz;
  tx0=tx;
  ty0=ty;
  tz0=tz;
  cont=cont+1;
  
  if (cont>10){    
    Serial.print(tx);
    Serial.print(",");
    Serial.print(ty);
    Serial.print(",");
    Serial.println(tz);
    Serial.println();
    cont=0;
  }
    
  delay(50);
}


void MPU6050_configuracion(int r1, int r2){
  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(r1);
  Wire.write(r2);
  Wire.endTransmission();
  delayMicroseconds(20);
}

void leerGiroscopio(){

  unsigned short int gyro_x_h=0, gyro_x_l=0, gyro_y_h=0, gyro_y_l=0, gyro_z_h=0, gyro_z_l=0;
  
  Wire.beginTransmission(MPU6050_ADDRESS); 
  Wire.write(MPU6050_GYRO_XOUT_H);
        
  Wire.requestFrom(MPU6050_ADDRESS, 6);
  gyro_x_h = Wire.read();
  gyro_x_l = Wire.read();
  gyro_y_h = Wire.read();
  gyro_y_l = Wire.read();
  gyro_z_h = Wire.read();
  gyro_z_l = Wire.read();
            
  Wire.endTransmission();
  gyro_x=(((int)gyro_x_h)<<8 )  |  ((int)gyro_x_l); 
  gyro_y=(((int)gyro_y_h)<<8 )  |  ((int)gyro_y_l); 
  gyro_z=(((int)gyro_z_h)<<8 )  |  ((int)gyro_z_l); 
}


void calibrar(){
  int i=0;
  for( i=0;i<1000;i++){
    leerGiroscopio();
    sumX=sumX+gyro_x;
    sumY=sumY+gyro_y;
    sumZ=sumZ+gyro_z;
    delay(1);
  } 
  sumX=sumX/1000;
  sumY=sumY/1000;
  sumZ=sumZ/1000;
}
