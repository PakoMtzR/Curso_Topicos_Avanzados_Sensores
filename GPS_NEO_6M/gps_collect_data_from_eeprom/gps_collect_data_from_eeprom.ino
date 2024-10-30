#include <EEPROM.h>

double latitude, longitude;
int i = 0;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("");
  for (int eeprom_addrs = 0; eeprom_addrs < EEPROM.length(); eeprom_addrs+=16) {
    i++;
    EEPROM.get(eeprom_addrs, latitude);
    EEPROM.get(eeprom_addrs+8, longitude);
    Serial.print(i);
    Serial.print(". ");
    Serial.print(latitude, 5);
    Serial.print(", ");
    Serial.println(longitude, 5);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
