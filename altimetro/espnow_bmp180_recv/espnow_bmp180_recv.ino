
#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  float temperature_esp32;
  float pressure_esp32;
  float altitude_esp32;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  // Serial.print("Bytes received: ");
  // Serial.println(len);
  // Serial.print("Temp[C]: ");
  // Serial.println(myData.temperature_esp32);
  // Serial.print("Pressure[mbar]: ");
  // Serial.println(myData.pressure_esp32);
  // Serial.print("Altitude[m]: ");
  // Serial.println(myData.altitude_esp32);
  // Serial.println();
  Serial.print(myData.temperature_esp32);
  Serial.print(",");
  Serial.print(myData.pressure_esp32);
  Serial.print(",");
  Serial.println(myData.altitude_esp32);
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}