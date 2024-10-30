/*
  Formato de datos recibidos por el GPS NEO 6M
  RMC - Recommended Minimum Navigation Information

  $GMRMC,hhmmss.ss,A,llll.ll,a,yyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh
  
  0) GMRMC
  1) Time (UTC)
  2) Status, V = Navigator receiver warning
  3) Latitude
  4) N or S
  5) Longitude 
  6) E or W
  7) Speed over ground, knots (1knot =1 nautical mile per hour，which is equivalent to 0.5m per second)
  8) Track made good, degrees true
  9) Date, ddmmyy
  10) Magnetic Variation, deegres
  11) E or W
  12) Checksum
*/

#include <SoftwareSerial.h>

const byte TX_PIN = 3;  // Pin de transmision (TX) para el GPS
const byte RX_PIN = 4;  // Pin de recepcion (RX) para el GPS
const unsigned int BAUD_RATE = 9600;  // Velocidad de baudios para las conexiones UART

char gps_data[100];       // Buffer para almacenar datos del GPS
char formattedData[100];  // Buffer para datos formateados

// Definir la conexion UART con el GPS
SoftwareSerial gps(RX_PIN, TX_PIN);

// Prototipo de funcion
void processGPSData(const char* data);

void setup() {
  Serial.begin(BAUD_RATE);  // Iniciar comunicacion serial para la depuracion
  gps.begin(BAUD_RATE);     // Iniciar comunicacion serial con el GPS
  delay(100);
}

void loop() {
  if (gps.available() > 0) {
    // Leer datos del GPS y procesarlos
    // Serial.readBytesUntil() reads characters from the serial buffer into an array. 
    // The function terminates if the terminator character is detected
    // [Syntax]: Serial.readBytesUntil(character, buffer, length)
    gps.readBytesUntil('\n', gps_data, sizeof(gps_data));
    processGPSData(gps_data);
  }
}

void processGPSData(const char* data) {
  // Comprobar si los datos comienzan con "$GPRMC"
  // La función strncmp se utiliza para comparar dos cadenas de caracteres hasta un número máximo de caracteres especificado.
  // [Syntax]: strncmp(str1, str2, num_caracteres)
  // Devuelve un valor negativo si str1 es menor que str2.
  // Devuelve cero si str1 es igual a str2.
  // Devuelve un valor positivo si str1 es mayor que str2.
  if (strncmp(data, "$GPRMC", 6) == 0) {
    char* token;
    char* rest = (char*)data;
    int i = 0;

    // Analizar los datos usando strtok_r en lugar de String::substring
    while ((token = strtok_r(rest, ",", &rest))) {
      if (i == 1) {
        // Obtener la hora
        strcpy(formattedData, token);
        strcat(formattedData, ",");
      } else if (i == 3 || i == 5) {
        // Convertir latitud y longitud
        float coordinate_f = atof(token);
        int hour = coordinate_f / 100;
        float hour_frac = (coordinate_f - (hour * 100)) / 60;
        float new_coordinate = hour + hour_frac;
        
        char buffer[10];
        dtostrf(new_coordinate, 4, 6, buffer);
        strcat(formattedData, buffer);
        strcat(formattedData, ",");
      } else if (i == 7) {
        // Obtener la velocidad
        float knots = atof(token);
        float velocity = knots * 0.5; // knots => m/s (aprox)
        char buffer[10];
        dtostrf(velocity, 4, 6, buffer);
        strcat(formattedData, buffer);
        strcat(formattedData, ",");
      } else if (i == 9) {
        // Obtener la fecha
        strcat(formattedData, token);
      }
      i++;
    }
    // Imprimir los datos formateados
    Serial.println(formattedData);
  }
}