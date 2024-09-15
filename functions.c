#include "functions.h"
#include "globals.h"
#include <util/delay.h>
#define F_CPU = 20000000UL

void inits() {
	TWI_initHost();
	USART0_init();
	PORT_init();
}

void delayms(int ms){
	while(ms){
		_delay_ms(1);
		ms--;
	}
}

char nibbleToHex(uint8_t data){
  data &= 0xF;
  if (data < 0xA) {return 0x30 + data;} //digits 0x0-9
  else return 0x37 + data; //digits 0xA-F
}

uint8_t hexToNibble(char hex){
  if ( (hex < 0x3A) && (hex > 0x2F) ) {return hex - 0x30;} //0x0-9
  else if ( (hex < 0x47) && (hex > 0x40) ) {return hex - 0x37;}//0xA-F
  else {return 'H';}
}

void PORT_init() {
	PORTC.DIRSET = (1<<2); //LED
	PORTB.DIRSET = PIN4_bm | PIN5_bm;
	PORTB.OUTCLR = PIN4_bm | PIN5_bm;
}
