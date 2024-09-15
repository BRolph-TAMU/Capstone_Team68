#include <avr/io.h>
#include "usart.h"
#include "globals.h"
#include <util/delay.h>



void USART0_init(){
	PORTB.DIRSET = PIN2_bm;
	PORTB.DIRCLR = PIN3_bm;
	USART0.BAUD = USART0_BAUD_RATE(19200);
	USART0.CTRLB |= USART_TXEN_bm;
	USART0.CTRLB |= USART_RXEN_bm;
}

void USART0_flush() {
	char c;
	while(USART0.STATUS & USART_RXCIF_bm){
		c = USART0.RXDATAL;
		_delay_us(10);
	}
}

void USART0_send(char c){
	while(!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = c;
}

void USART0_sendString(char* str, uint8_t len){
	for(uint8_t i = 0; i < len; i++){
		if (str[i]=='\0') {
			USART0_send('\n');
			break;
		}
		USART0_send(str[i]);
		if (str[i]=='\n') {break;}
		
	}
}
	
void USART0_read(){
	char c = 0;
	uint8_t index = 0;
	
	while(index < USART_LEN){
		while(!(USART0.STATUS & USART_RXCIF_bm));
		c = USART0.RXDATAL;
		if (c== '\n'){break;}
		USART0_BUFF[index++] = c;
	}
}
