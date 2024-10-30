#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 4 // Pin del botón

uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0xD7, 0xE5, 0x38}; // Dirección MAC del receptor

volatile bool ledState = false;

// Estructura de mensaje para enviar datos
typedef struct struct_message {
  bool ledState;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback cuando se envían los datos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Manejador de la interrupción del botón
void IRAM_ATTR handleButtonPress() {
  ledState = !ledState;
  myData.ledState = ledState;
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  Serial.println(ledState);
}

void setup() {
  // Inicializar el monitor serie
  Serial.begin(115200);

  // Configurar el pin del botón como entrada con resistencia pull-down
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);

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
}

void loop() {
  // El loop se mantiene vacío ya que el manejo del botón se realiza con interrupciones
}
