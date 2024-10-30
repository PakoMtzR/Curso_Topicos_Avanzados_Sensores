#define   START      0x08 // Maestro envía START   // son constantes a lolargo de todo el programa
#define RESTART      0x10 // Maestro envía ReSTART o cambio de dirección de nformación

#define MT_SLA_ACK   0x18 // Maestro indica que va escribir y esclavo contesta 
#define MR_SLA_ACK   0x40 // Maestro indica que va leer y esclavo contesta

#define MT_DATA_ACK  0x28 // Maestro envía datos y esclavo contesta
#define MR_DATA_ACK  0x50 // Maestro contesta a esclavo que recibío los datos

#define MR_DATA_NACK 0x58 // Maestro recibe datos pero genera un NACK

void I2C_init(){
    TWSR=0x00; // PReescaler de 1
    TWBR=0x03; //400Khz
    TWCR=(1<<TWEN); // Habilitando el I2C
}

uint8_t I2C_start(){ 
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);// Limpiar TWINT, TWSTAR =1 y habiliatar TWI  
    while (!(TWCR & (1<<TWINT)));          // Esperamos a que el START se haya enviado correctamente //(TWCR & (1<<TWINT))==0
    if (((TWSR & 0xF8) != START) || ((TWSR & 0xF8) != RESTART)) 
        return 0;
    else   
        return 1;
}

uint8_t I2C_slave(uint8_t SLA_WR){
    TWDR = SLA_WR;                  // Cargamos la dirección del Esclavo
    TWCR = (1<<TWINT)|(1<<TWEN);   // Limpiamos bandera de INT y habilitamos I2C
    while (!(TWCR & (1<<TWINT))) ; // Esperamos a que la dirección se haya enviado correctamente 
    
    // Comprobar registros
    if (((TWSR & 0xF8) != MT_SLA_ACK) || ((TWSR & 0xF8) != MR_SLA_ACK))  // Verificando que el esclavo recibió la dirección
        return 0;    
    else 
        return 1;   
}

uint8_t I2C_write(uint8_t DATA){
    TWDR = DATA;                 // Cargando registro con los datos a enviar
    TWCR =(1<<TWINT)|(1<<TWEN);  // Limpiando Bandera de INT y Habilitando I2C
    while (!(TWCR & (1<<TWINT))); // Esperando a que los datos hayan sido enviados
    
    if ((TWSR & 0xF8) != MT_DATA_ACK)
        return 0;
    else
        return 1;   
}

uint8_t I2C_read_ack(){
    uint8_t DATA;
                    
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA); // Contesto con Acknowledge  
    while (!(TWCR & (1<<TWINT))); 
    DATA = TWDR;   
    
    if ((TWSR & 0xF8) != MR_DATA_ACK)  // Todo llego bien 
        return 0;
    else
        return DATA; 
}

uint8_t I2C_read_nack(){
    uint8_t DATA;
                    
    TWCR =(1<<TWINT)|(1<<TWEN); // Contesto con Acknowledge  
    while (!(TWCR & (1<<TWINT))); 
    DATA = TWDR;   
    
    if ((TWSR & 0xF8) != MR_DATA_NACK)  // Todo llego bien 
        return 0;
    else
        return DATA; 
}

void I2C_stop(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); // Limpiar Bandera INT, Habilitar I2C y Habilitar Bit Stop
}