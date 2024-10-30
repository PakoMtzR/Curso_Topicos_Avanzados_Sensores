void USART_init(void){
    UBRR0H = 0X00;
    UBRR0L = 0x67;
    UCSR0C |= (1<<UCSZ01)|(1<UCSZ00);
    UCSR0A |= (1<<TXEN0);
}
void USART_txch(char ch){
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0=ch;
}

void USART_txstr(char *str){
    uint8_t i=0;
    while(str[i]!='\0'){
        USART_txch(str[i]);
        i++;
    }
}