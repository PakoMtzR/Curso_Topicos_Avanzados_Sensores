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

// Definir la conexion UART con el GPS
SoftwareSerial gps(RX_PIN, TX_PIN);

// Prototipo de funcion
float convert_to_HHhh(String coordinate);
String adjust_date_time(String date, String time);

void setup() {
  Serial.begin(BAUD_RATE);  // Iniciar comunicacion serial para la depuracion
  gps.begin(BAUD_RATE);     // Iniciar comunicacion serial con el GPS
  delay(100);
}

void loop() {
  if (gps.available() > 0) {
    String gps_data = gps.readStringUntil('\n');
    if (gps_data.startsWith("$GPRMC")) {
      int commaIndex = -1;
      String data[13];
      byte i = 0;
      
      while (gps_data.length() > 0) {
        commaIndex = gps_data.indexOf(',');
        if (commaIndex == -1) {
          data[i] = gps_data;
          break;
        }
        else {
          data[i] = gps_data.substring(0, commaIndex);
          gps_data = gps_data.substring(commaIndex + 1);
          i++;
        }
      }

      if (i >= 12) {
        float latitude = convert_to_HHhh(data[3]);
        float longitude = convert_to_HHhh(data[5]);
        float speed = atof(data[7].c_str()) * 0.514444; // 1 knot = 0.514444 m/s
        
        char buffer[15]; // Suficiente para coordenadas y velocidad
        
        memset(buffer, 0, sizeof(buffer));  // Limpiar el buffer antes de su reutilización
        dtostrf(latitude, 4, 6, buffer);
        String formattedLatitude = buffer;
        
        memset(buffer, 0, sizeof(buffer));  // Limpiar el buffer antes de su reutilización
        dtostrf(-longitude, 4, 6, buffer);  // Longitud negativa (Oeste)
        String formattedLongitude = buffer;
        
        memset(buffer, 0, sizeof(buffer));  // Limpiar el buffer antes de su reutilización
        dtostrf(speed, 4, 6, buffer);
        String formattedSpeed = buffer;

        //String formattedTime = data[1].substring(0, 2) + ":" + data[1].substring(2, 4) + ":" + data[1].substring(4, 6);
        //String formattedDate = data[9].substring(0, 2) + "/" + data[9].substring(2, 4) + "/" + data[9].substring(4, 6);
        
        String adjustedDateTime = adjust_date_time(data[9], data[1]);

        //String formattedData = formattedDate + "," + formattedTime + "," + formattedLatitude + "," + formattedLongitude + "," + formattedSpeed;
        String formattedData = adjustedDateTime + "," + formattedLatitude + "," + formattedLongitude + "," + formattedSpeed;

        Serial.println(formattedData);
      }
    }
  }
}

float convert_to_HHhh(String coordinate) {
  float coordinate_f = atof(coordinate.c_str());
  int hour = coordinate_f / 100;
  float hour_frac = (coordinate_f - (hour * 100)) / 60;
  return hour + hour_frac;
}

String adjust_date_time(String date, String time) {
  // Extraer la fecha actual del formato DDMMYY a MMDDYY
  int day = date.substring(0, 2).toInt();
  int month = date.substring(2, 4).toInt();
  int year = date.substring(4, 6).toInt();
  int hour = time.substring(0,2).toInt() - 6;
  //hour -= 6;
  if (hour < 0) {
    hour += 24;
    // Ajustar la fecha si la hora se vuelve negativa
    // Verificar si restar 6 horas cambia el día
    if (day > 1) {
      day -= 1; // Restar un día
    } else {
      // Restar un mes y ajustar el día al último día del mes anterior
      month -= 1;
      if (month == 0) {
        month = 12; // Si estamos en enero, retroceder al último mes del año anterior
        year -= 1;
      }
      // Determinar el último día del mes anterior
      if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        day = 31;
      } else if (month == 2) {
        // Año bisiesto
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
          day = 29;
        } else {
          day = 28;
        }
      } else {
        day = 30;
      }
    }
  }
  // Reconstruir la fecha ajustada
  date = (day < 10 ? "0" : "") + String(day) + "/" + (month < 10 ? "0" : "") + String(month) + "/" + (year < 10 ? "0" : "") + String(year);
  return date + "," + (hour < 10 ? "0" : "") + String(hour) + ":" + time.substring(2, 4) + ":" + time.substring(4, 6);
}