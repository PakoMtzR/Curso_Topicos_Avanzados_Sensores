#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2 // Pin del LED

// Estructura de mensaje para recibir datos
typedef struct struct_message {
  bool ledState;
} struct_message;

struct_message myData;

// Callback cuando se reciben los datos
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  digitalWrite(LED_PIN, myData.ledState ? HIGH : LOW);
  Serial.print("LED State: ");
  Serial.println(myData.ledState ? "ON" : "OFF");
}

void setup() {
  // Inicializar el monitor serie
  Serial.begin(115200);

  // Configurar el pin del LED como salida
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Configurar el dispositivo como una estación Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Registrar el callback para recibir datos
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // El loop se mantiene vacío ya que el manejo de datos se realiza mediante callbacks
}
