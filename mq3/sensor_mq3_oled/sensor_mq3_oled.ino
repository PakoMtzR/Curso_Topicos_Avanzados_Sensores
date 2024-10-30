#include <Wire.h>
#include <Adafruit_GFX.h>   //Liberia para gráficos
#include <Adafruit_SSD1306.h>  //Liberia para Oleds monocromáticos basados ​​en controladores SSD1306
/*Se declara el pin de reset, este es requerido por el constructor de la librería SSD1306
 *para definir el tipo de OLED y la comunicación I2C en los (pines SDA, SCL)*/
#define OLED_RESET -1 
Adafruit_SSD1306 display(OLED_RESET);
/*Se define una condición para saber si en la librería esta definida la altura de 32 de lo contrario 
 *no dejara compilar el código hasta que se cambie la altura correcta desde la librería*/
#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta, cambie en la librería de Adafruit_SSD1306.h!");
#endif

// Pines para el sensor de alcohol MQ-3
const int sensorPin = A0; // Pin A0 para el sensor de alcohol
const int voltagePin = A1; // Pin A1 para la referencia de voltaje

// Variables empleadas
float div_voltaje = 0.0;
float rs_ro = 0.0;
float alcohol = 0.0;
float vout_sensor = 0.0;

const float factor = 5.0 / 1023.0; // Factor de conversión para el ADC (Arduino tiene resolución de 10 bits)

void setup() {
  // Inicializar la comunicación serie
  Serial.begin(9600);

   /* Se inicia la comunicación I2C en la direccion 0x3C para la pantalla oled 128x32*/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  delay(2000); // Pausa para mostrar mensaje de inicio

  // Limpiar la pantalla
  display.clearDisplay();
}

void loop() {
  // Lectura del sensor
  vout_sensor = analogRead(sensorPin) * factor;
  div_voltaje = analogRead(voltagePin) * factor;

  // Cálculo del cociente Rs/Ro
  if (vout_sensor != 0) {
    rs_ro = (5.0 / vout_sensor) - 1;
  } else {
    rs_ro = 0; // Evitar división por cero
  }

  // Cálculo de la concentración de alcohol
  alcohol = 0.354 * pow(rs_ro, -1.518);

  // Imprimir en el monitor serie la concentración de alcohol en el aire
  Serial.print("Concentracion de alcohol: ");
  Serial.print(alcohol);
  Serial.println(" mg/L");
  Serial.println(div_voltaje);

  // Limpiar la pantalla OLED
  display.clearDisplay();

  // Mostrar los datos en la pantalla OLED
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Alcohol: ");
  display.print(alcohol);
  display.println(" mg/L");
  display.setCursor(0, 10);
  display.print("Voltaje: ");
  display.print(div_voltaje);
  display.println(" V");

  // Actualizar la pantalla OLED
  display.display();

  delay(100); // Retardo por estabilidad 
}

