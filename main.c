#include <avr/io.h>
#include "twi.h"
#include "functions.h"
#include "usart.h"
#include "globals.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>


int main(){

	inits();
	USART0_flush();
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); //No prescaler, 20 MHz clock
	char message[5] = {'\n','\n','\n','\n','\n'};
	int angle = 0;
	while(1){
		PORTC.OUTSET = (1<<2);
		delayms(60);
		PORTC.OUTCLR = (1<<2);
		delayms(60);
		TWI_sendAndReadBytes(0x36, 0x0E, TWI_BUFF, TWI_LEN);
		for (uint8_t i = 0; i < TWI_LEN; i++) {
			//message[i] = nibbleToHex(TWI_BUFF[i]);
			angle += (TWI_BUFF[i] << 8*(TWI_LEN-1-i));
		}
		itoa(angle, message, 10);
		angle = 0;
		USART0_sendString(message, 5);
		PORTB.OUTTGL = PIN4_bm;
		//USART0_send('\n');
	}
	return 0;
}



