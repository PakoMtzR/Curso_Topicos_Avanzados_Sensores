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

// Definimos los pines para la conexion UART del GPS
const byte TX = 3;
const byte RX = 4;

String data[13];
String gps_data = "";
String date, time, latitude, longitude, speed;
String formattedData = "";

byte i = 0;
int commaIndex = -1;

// Establecemos los pines para la conexion UART con el gps
SoftwareSerial gps(RX, TX);

String convert_to_HHhh(String coordinate);
String convert_knots2mpersecond(String knots_speed);
String calibrate_time(String time);
String calibrate_date(String date);

void setup() {
  // Inicializamos las conexiones UART
  Serial.begin(9600);
  gps.begin(9600);
}

void loop() {
  if (gps.available() > 0) {
    gps_data = gps.readStringUntil('\n');
    if (gps_data.startsWith("$GPRMC")) {
      i = 0;
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
        time = calibrate_time(data[1]);
        latitude = convert_to_HHhh(data[3]);
        longitude = convert_to_HHhh(data[5]);
        speed = convert_knots2mpersecond(data[7]);
        date = calibrate_date(data[9]);
        formattedData = date + "," + time + "," + latitude + ",-" + longitude + "," + speed;
        Serial.println(formattedData);
      }
    }
  }
}

String convert_to_HHhh(String coordinate) {
  float coordinate_f = atof(coordinate.c_str());
  int hour = coordinate_f / 100;
  float hour_frac = (coordinate_f - (hour * 100)) / 60;
  float new_coordinate = hour + hour_frac;

  // Convertir new_coordinate a string
  char buffer[10]; // Tamaño suficiente para contener el valor máximo del float
  dtostrf(new_coordinate, 4, 6, buffer); // Usar dtostrf para convertir el float a string
  
  return String(buffer); // Devolver el string resultante
}

String convert_knots2mpersecond(String knots_speed) {
  float speed = atof(knots_speed.c_str()) * 0.514444; // 1 nudo = 0.514444 m/s
  char buffer[10];
  dtostrf(speed, 4, 6, buffer);
  return String(buffer);
}

String calibrate_time(String time) {
  // String time_str = time;
  return time.substring(0, 2) + ":" + time.substring(2, 4) + ":" + time.substring(4, 6);
}

String calibrate_date(String date) {
  // String date_str = date;
  return date.substring(0, 2) + "/" + date.substring(2, 4) + "/" + date.substring(4, 6);
}