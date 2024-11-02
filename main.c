#include <avr/io.h>
#include "twi.h"
#include "functions.h"
#include "usart.h"
#include "globals.h"
#include "timers.h"
#include "pid.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

PID panPID;
PID tiltPID;
char message[5] = {'\n','\n','\n','\n','\n'};
char pidmsg[5] = {'p', 'i', 'd',':',' '};
char angmsg[5] = {'a', 'n', 'g',':',' '};
char intmsg[9] = {'p','e','n','i','s','o','o','o','\n'};
int16_t lastval = 0;
uint8_t PIT_index = 0;
int main(){
	sei(); //enable interrupts
	inits();
	USART0_flush();
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); //No prescaler, 20 MHz clock

	
	PID_init(&panPID, 200, 1, 0, 4000);
	PID_init(&tiltPID, 0, 0, 0, 0);
	TCA0.SPLIT.LCMP1 = 0x0;
	while(1){
		getCommand();
	}

	return 0;
}



//periodic interrupt timer ISR.
ISR(RTC_PIT_vect){
	RTC.PITINTFLAGS = 0x1; //clear interrupt flag
	
	//TCA0.SPLIT.LCMP1 = PID_compute(&panPID, readPos(PAN));
	uint16_t angle = readPos(PAN);
	
	int32_t pidval = PID_compute(&panPID, (int) angle);

	motorSet(pidval, 1);
	if(PIT_index++ == 0xF) {
		PIT_index = 0;
		intToHexChar(intmsg, angle);
		intmsg[0] = 'A';
		USART0_sendString(intmsg, 9);
		intToHexChar(intmsg, pidval);
		intmsg[0] = 'P';
		USART0_sendString(intmsg, 9);
	}
	
}
//usart0 receive complete
ISR(USART0_RCX_vect){
}
