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

void setup() {
 /* Se inicia la comunicación I2C en la direccion 0x3C para la pantalla oled 128x32*/
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}
void loop() {
/*Se manda a llamar la función leer_oled para ejecutar su contenido*/
leer_oled();
}
/*Se declara la función leer_oled la cual define el tamaño, color, texto y la posición del texto 
que se mostrara en el display oled*/
void leer_oled() { 
  display.clearDisplay(); //Borra el buffer
  display.setTextSize(1); //Establece el tamaño de fuente, admite tamaños de 1 a 8
  display.setTextColor(WHITE); //Establece el color 
  display.setCursor(0,10); //Establecer las coordenadas para mostrar la posición del texto
  display.println("UNIT ELECTRONICS");
  display.setTextSize(1);
  display.setCursor(20,20); //Establecer las coordenadas para mostrar la posición del texto
  display.println("PARA TI ;)"); 
  display.display(); //Muestra el texto 
}
