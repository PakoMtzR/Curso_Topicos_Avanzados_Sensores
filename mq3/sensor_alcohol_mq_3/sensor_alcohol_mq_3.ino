//********************************Sensor de Alcohol**********************************
//*                                                                                 *
//*El MQ-3 es un sensor que mide la concentración en mg/L de alcohol en el aire.    *
//*                                                                                 *
//*El siguiente programa realiza la lectura del sensor de alcohol MQ-3 y en base a  *
//*este dato calcula la concentración en mg/L en el aire.                           *
//***********************************************************************************

//Se declaran las variables empleadas
int readSensor;
float div_voltaje;
float rs_ro, alcohol, vout;

const float factor = 0.00488;

void setup()
{
  Serial.begin(9600);    //Se inicia la comunicación serie a 9600 baudios
}

void loop()
{
  readSensor = analogRead(A0);  //Lectura del sensor
  div_voltaje = analogRead(A1) * factor;
  vout = factor * readSensor;  //Conversión a volts. La resolución es 5V/1024 = 0.00488mV
  rs_ro = (5/vout) - 1;  //Cálculo del cociente Rs/Ro. Esta ecuación se obtiene del 
  //analisis del circuito realizada en el Desarrollo.
  alcohol = 0.354*pow(rs_ro, -1.518);  //Conforme a la Fig. 2 de la hoja de datos del
  //sensor MQ-3 se realiza la regresión en Excel, obteniendose así su ecuación
  //caracteristíca: mg/L de Alcohol = 0.354*(RS/RO)^-1.518
  
  //Se imprime en el monitor serie la concentración de alcohol en el aire
  Serial.print("Concentracion de alcohol: ");  
  Serial.print(alcohol);     
  Serial.println(" mg/L");
  Serial.println(div_voltaje);
  
  delay(10);  //Retardo por estabilidad
}


