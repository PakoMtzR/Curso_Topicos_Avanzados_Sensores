#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET -1 
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta, cambie en la librería de Adafruit_SSD1306.h!");
#endif

const byte TX = 3;
const byte RX = 4;

const byte LED = 7;

String data[13];
String gps_data = "";
String date, time, latitude, longitude; 
String formattedData = "";

byte i = 0;
int commaIndex = -1;

byte required_data = 100;
int eeprom_addr = 0;

// Establecemos los pines para la conexion UART con el gps
SoftwareSerial gps(RX, TX);

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Inicializamos las conexiones UART
  Serial.begin(9600);
  gps.begin(9600);

  // Iniciamos la comunicacion I2C con la pantalla oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  // Configuramos la pantalla oled
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  delay(100);
}

void loop() {
  if (gps.available() > 0 && required_data > 0) {
    gps_data = gps.readStringUntil('\n');
    if (gps_data.startsWith("$GPRMC")) {
      Serial.println(gps_data);
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
        date = data[9];
        time = data[1];
        latitude = data[3];
        longitude = data[5];

        if (latitude != "" && longitude != "") {
          formattedData = "[Date]:" + date + " [Time]:" + time + "\n[Long]: " + longitude + "\n[Latd]: " + latitude + "\n";
          EEPROM.put(eeprom_addr, latitude.toDouble());
          EEPROM.put(eeprom_addr + 8, longitude.toDouble());
          eeprom_addr += 16;
          required_data -= 2;
        }
        else {
          formattedData = "Localizando...";
        }
      }
      display.clearDisplay();
      display.setCursor(0,0);
      display.println(formattedData);
      display.display();
    }
  }
  if (required_data == 0) {
    digitalWrite(LED, HIGH);
    // byte k = 0;
    // for (int j = 0; j < EEPROM.length(); j+=22) {
    //   EEPROM.get(j, latitude);
    //   EEPROM.get(j+11, longitude);

    //   k++;
    //   Serial.print(k);
    //   Serial.print(". ");
    //   Serial.print(latitude);
    //   Serial.print(", ");
    //   Serial.println(longitude);
    // }
  }

}
