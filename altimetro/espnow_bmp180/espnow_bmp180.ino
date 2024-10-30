#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0xD7, 0xE5, 0x38}; // Dirección MAC del receptor

// Estructura de mensaje para enviar datos
typedef struct struct_message {
  float temperature_esp32;
  float pressure_esp32;
  float altitude_esp32;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback cuando se envían los datos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


#define ADDRESS_SENSOR 0x77                 // Sensor address PAGNI9NA 20

int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md; //p´gina 15 Store sensor PROM values from BMP180
uint16_t ac4, ac5, ac6;                     // Store sensor PROM values from BMP180
// Ultra Low Power       OSS = 0, OSD =  5ms //pagina 12
// Standard              OSS = 1, OSD =  8ms
// High                  OSS = 2, OSD = 14ms
// Ultra High Resolution OSS = 3, OSD = 26ms
const uint8_t oss = 3;                      // Set oversampling setting PAGINA 21
const uint8_t osd = 26;                     // with corresponding oversampling delay 

float T, P;                                 // Set global variables for temperature and pressure 

void setup()
{
  Serial.begin(115200);                       // Set up serial port
  while(!Serial){;}                         // waith until leonardo serial port ready
  delay(5000);
  Wire.begin();                             // Activate I2C
  
  init_SENSOR();                            // Initialize baro sensor variables ALGORITMO PAGINA 15

  // Configuracion de ESP-NOW
  // Configurar el dispositivo como una estación Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Registrar el callback para el estado de envío
  esp_now_register_send_cb(OnDataSent);

  // Registrar el peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Agregar el peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  delay(100);
}

void loop()
{
  int32_t b5;

  b5 = temperature();                       // Read and calculate temperature (T) 

  // Serial.print("Temperature: ");
  // Serial.print(T, 2);
  // Serial.print(" C, ");
  // Serial.print(1.8 * T + 32.0, 2);
  // Serial.println(" F");

  P = pressure(b5);                         // Read and calculate pressure (P) 

  // Serial.print("Pressure: ");
  // Serial.print(P, 2);
  // Serial.print(" mbar, ");
  // Serial.print(P * 0.0295299830714, 2);
  // Serial.println(" inHg");
    
  float alt = (pow(1013.25 / P, 0.190223f) - 1.0f) * 44330.08f;                       // Use static SI standard temperature (15°C) 
  
  // Serial.print("Altitude: "); Serial.print(alt); Serial.print(" m, "); Serial.print(alt * 3.28084); Serial.println(" ft");
  // Serial.println("-----------------------------------------");

  myData.temperature_esp32 = T;
  myData.pressure_esp32 = P;
  myData.altitude_esp32 = alt;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Send success");
  } else {
    Serial.println("Send fail");
  }

  delay(500);                               // Delay between each readout
}

/**********************************************
  Initialize sensor variables
**********************************************/
void init_SENSOR()
{
  ac1 = read_2_bytes(0xAA);//NINGUNA DE STAS VARIABLES DEBE TENER EL VALOR DE CERO
  ac2 = read_2_bytes(0xAC);
  ac3 = read_2_bytes(0xAE);
  ac4 = read_2_bytes(0xB0);
  ac5 = read_2_bytes(0xB2);
  ac6 = read_2_bytes(0xB4);
  b1  = read_2_bytes(0xB6);
  b2  = read_2_bytes(0xB8);
  mb  = read_2_bytes(0xBA);
  mc  = read_2_bytes(0xBC);
  md  = read_2_bytes(0xBE);

/*
  Serial.println("");
  Serial.println("Sensor calibration data:");
  Serial.print(F("AC1 = ")); Serial.println(ac1);
  Serial.print(F("AC2 = ")); Serial.println(ac2);
  Serial.print(F("AC3 = ")); Serial.println(ac3);
  Serial.print(F("AC4 = ")); Serial.println(ac4);
  Serial.print(F("AC5 = ")); Serial.println(ac5);
  Serial.print(F("AC6 = ")); Serial.println(ac6);
  Serial.print(F("B1 = "));  Serial.println(b1);
  Serial.print(F("B2 = "));  Serial.println(b2);
  Serial.print(F("MB = "));  Serial.println(mb);
  Serial.print(F("MC = "));  Serial.println(mc);
  Serial.print(F("MD = "));  Serial.println(md);
  Serial.println("");
*/
}

/**********************************************
  Calcualte pressure readings
**********************************************/
float pressure(int32_t b5)
{
  int32_t x1, x2, x3, b3, b6, p, UP;
  uint32_t b4, b7; 

  UP = read_pressure();                          // Read raw pressure
 // Serial.print(F("UP = ")); Serial.print(UP); Serial.println(F(";"));

  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 >> 12)) >> 11; 
  x2 = ac2 * b6 >> 11;
  x3 = x1 + x2;
  int32_t b3_tmp = ac1;                          // Improved b3 calculation which also uses less program code.
  b3_tmp = (b3_tmp * 4 + x3) << oss;
  b3     = (b3_tmp + 2) >> 2;
  x1 = ac3 * b6 >> 13;
  x2 = (b1 * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)UP - b3) * (50000 >> oss);
  if(b7 < 0x80000000) { p = (b7 << 1) / b4; } else { p = (b7 / b4) << 1; } // or p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  return (p + ((x1 + x2 + 3791) >> 4)) / 100.0f; // Return pressure in mbar
}


/**********************************************
  Read uncompensated temperature
**********************************************/
int32_t temperature()
{
  int32_t x1, x2, b5, UT;

  Wire.beginTransmission(ADDRESS_SENSOR); // Start transmission to device 
  Wire.write(0xf4);                       // Sends register address
  Wire.write(0x2e);                       // Write data
  Wire.endTransmission();                 // End transmission
  delay(5);                               // Datasheet suggests 4.5 ms
  
  UT = read_2_bytes(0xf6);                // Read uncompensated TEMPERATURE value

  // Calculate true temperature
  x1 = (UT - (int32_t)ac6) * (int32_t)ac5 >> 15;
  x2 = ((int32_t)mc << 11) / (x1 + (int32_t)md);
  b5 = x1 + x2;
  T  = (b5 + 8) >> 4;
  T = T / 10.0;                           // Temperature in celsius 
  return b5;  
}

/**********************************************
  Read uncompensated pressure value
**********************************************/
int32_t read_pressure()
{
  int32_t value; 
  Wire.beginTransmission(ADDRESS_SENSOR);   // Start transmission to device 
  Wire.write(0xf4);                         // Sends register address to read from
  Wire.write(0x34 + (oss << 6));            // Write data
  Wire.endTransmission();                   // SEd transmission
  delay(osd);                               // Oversampling setting delay
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(0xf6);                         // Register to read
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_SENSOR, 3);      // Request three bytes
  if(Wire.available() >= 3)
  {
    value = (((int32_t)Wire.read() << 16) | ((int32_t)Wire.read() << 8) | ((int32_t)Wire.read())) >> (8 - oss);
  }
  return value;                             // Return value
}

/**********************************************
  Read 1 byte from the BMP sensor
**********************************************/
uint8_t read_1_byte(uint8_t code)
{
  uint8_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 1);            // Request data for 1 byte to be read
  if(Wire.available() >= 1)
  {
    value = Wire.read();                          // Get 1 byte of data
  }
  return value;                                   // Return value
}

/**********************************************
  Read 2 bytes from the BMP sensor
**********************************************/
uint16_t read_2_bytes(uint8_t code)
{
  uint16_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 2);            // Request 2 bytes from device
  if(Wire.available() >= 2)
  {
    value = (Wire.read() << 8) | Wire.read();     // Get 2 bytes of data
  }
  return value;                                   // Return value
}