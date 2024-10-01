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

int main(){
	sei(); //enable interrupts
	inits();
	USART0_flush();
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); //No prescaler, 20 MHz clock
	char message[5] = {'\n','\n','\n','\n','\n'};
	
	PID_init(&panPID, 0, 0, 0, 0);
	PID_init(&tiltPID, 0, 0, 0, 0);
	while(1){

		//itoa(angle, message, 10);
	}

	return 0;
}




ISR(RTC_PIT_vect){
	RTC.PITINTFLAGS = 0x1; //clear interrupt flag
	PORTC.OUTTGL = (1<<2);
}
