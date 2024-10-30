//**************************************** GPS *******************************************
//*                                                                                      *
//*El GPS es un dispositivo que mediante la triangulación satelital puede calcular la    *
//*su posición con una gran precisión.                                                   *
//*                                                                                      *
//*En el Arduino obtendremos y procesaremos la cadena del GPS, para poder determinar la  *
//*posición (latitud y longitud), la hora GMT, la fecha y la velocidad.                  *
//*                                                                                      *
//*Para más detalle consulte el protocolo NMEA                                           *
//****************************************************************************************

//Definimos las variables utilizadas en el programa
char findString, rmc[100], data, hora[11], lat[10], lon[11];
char vel[6], fecha[7], senLN, senLT;
int contComa = 0, cH = 0, cLT = 0, cLN = 0, cVel = 0, cFecha = 0;
float gLat, sLat, gLon, sLon, convVel;

void setup()
{
  Serial.begin(9600);            //Comunicación serie a 9600 baudios (Velocidad del GPS GTPA013)
}

void loop()
{
  stringRMC();                   //Función para tomar la cadena GPRMC ->hora, latitud, longitud, velocidad y fecha
  
  cH = 0;                        //Contadores para tomar cada uno de los datos utiles de la cadena
  cLT = 0;
  cLN = 0;
  cVel = 0;
  cFecha = 0;
  contComa = 0;
  
  while(true)
  {
    if (Serial.available() > 0)  //Si recibimos algun valor del GPS
    {
      
      data = Serial.read();      //Leemos el caracter
      if (data == ',')           //Detectamos las comas para saber que tipo de datos se toma (hora, fecha, etc.)
        contComa ++;             //Contamos el numero de comas
      else if (contComa == 1)    //Después de una coma en la cadena se encuentra la HORA GMT
      {
        hora[cH] = data;         //Almacenamos la hora en un vector
        cH ++;
      }
      else if (contComa == 3)    //Después de la tercer coma se encuentra la LATITUD
      {
        lat[cLT] = data;         //Almacenamos la latitud en un vector 
        cLT ++;        
      }
      else if (contComa == 4)   //Después de la cuarta coma se encuentra el sentido de la LATITUD (N->norte, S->sur)
        senLT = data;
      else if (contComa == 5)   //La quinta coma indica la LONGITUD
      {
        lon[cLN] = data;        //Almacenamos la longitud en un vector
        cLN ++; 
      }
      else if (contComa == 6)   //La sexta coma indica el sentido de la LONGITUD (W->oeste, E->este)
        senLN = data;
      else if (contComa == 7)   //La septima coma indica la VELOCIDAD en nudos a la que se desplaza el GPS.
      {
        vel[cVel] = data;       //Almacenamos la velocidad en un vector
        cVel ++;
      }
      else if (contComa == 9)   //Después de la novena coma leemos la FECHA 
      {
        fecha[cFecha] = data;   //Almacenamos la fecha en un vector
        cFecha ++;
      }
      if (data == '*')          //Cuando leamos el caracter '*' finalizamos la lectura, pues indica el fin de la cadena GPRMC
        break;
    }
  }
  
  //Hora GMT -> Formato HH:MM:SS.SS
  Serial.print("Hora: ");
  //Hora
  Serial.print(hora[0]);
  Serial.print(hora[1]);
  Serial.print(":");
  //Minutos
  Serial.print(hora[2]);
  Serial.print(hora[3]);
  Serial.print(":"); 
  //Segundos
  Serial.print(hora[4]);
  Serial.println(hora[5]);
  
  //Latitud -> Formato GG°MM'SS"  
  Serial.print("Latitud: ");
  //Obtención de los segundos
  gLat = atof(lat);
  sLat = (gLat - floor(gLat))*60;
  //Grados
  Serial.print(lat[0]);
  Serial.print(lat[1]);
  Serial.print(" ");
  //Minutos
  Serial.print(lat[2]);
  Serial.print(lat[3]);
  Serial.print("'"); 
  //Segundos
  Serial.print(sLat);
  Serial.print("''");
  //Sentido
  Serial.println(senLT);  
  
  //Longitud -> Formato GG°MM'SS"  
  Serial.print("Longitud: ");
  //Obtención de los segundos
  gLon = atof(lon);
  sLon = (gLon - floor(gLon))*60;
  //Grados
  Serial.print(lon[0]);
  Serial.print(lon[1]);
  Serial.print(lon[2]);
  Serial.print(" ");
  //Minutos
  Serial.print(lon[3]);
  Serial.print(lon[4]);
  Serial.print("'"); 
  //Segundos
  Serial.print(sLon);
  Serial.print("''");
  //Sentido
  Serial.println(senLN);    

  //Velocidad en km/h
  Serial.print("Velocidad: ");
  //Conversión de nudos a km/h, 1 nudo = 1.852 km/h 
  convVel = atof(vel) * 1.852; 
  //Velocidad en nudos
  Serial.print(convVel);
  Serial.println(" km/h");
  
  //Fecha -> DD/MM/AA 
  Serial.print("Fecha: ");
  Serial.print(fecha[0]);
  Serial.print(fecha[1]);
  Serial.print("/");
  Serial.print(fecha[2]);
  Serial.print(fecha[3]);
  Serial.print("/");
  Serial.print(fecha[4]);
  Serial.println(fecha[5]);  
  Serial.println("");
  
}

//Función que detecta la cadena $GPRMC
void stringRMC()
{
  while(true)
  {
    if (Serial.available() > 0)            //Mientras tengamos datos por leer
    {
      findString = Serial.read();          //Leemos el GPS
      if (findString == 'R')               //Si el caracter leído es la letra 'R' continuamos la lectura
      {
        while (Serial.available() == 0);   //Si recibimos un caracter y es la letra 'M' continuamos la lectura
        findString = Serial.read();
        if (findString == 'M')
        {
          while (Serial.available() == 0); //Si recibimos un caracter y es la letra 'C' continuamos leyendo
          findString = Serial.read();      //si no es así seguiremos verificando al GPS hasta cumplir esta 
          if (findString == 'C')           //ultima condición
            break;
        }
      }
    }
  }
}

