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
char intmsg[9] = {'\n','\n','\n','\n','\n','\n','\n','\n','\n'};
int16_t lastval = 0;
uint8_t PIT_index = 0;
int main(){
	sei(); //enable interrupts
	inits();
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); //No prescaler, 20 MHz clock
	uint16_t commandReturn = 0;
	PID_init(&panPID, 0x80, 0, 0, 0x400);
	PID_init(&tiltPID, 0x80, 0, 0, 0xF00);
	delayms(100);
	USART0_init();
	PORTA.OUTCLR = LEDPIN;
	panPID.minspeed = findMinSpeed(PAN);
	tiltPID.minspeed = findMinSpeed(TILT);

	RTC_init(); //wait til now to start so that it doesnt interrupt findminspeed
	delayms(100);
	USART0_flush();
	while(1){
		commandReturn = getCommand();
		intToHexChar(intmsg, commandReturn);
		intmsg[0] = 'M';
		USART0_sendString(intmsg, 9);
	}

	return 0;
}



//periodic interrupt timer ISR.
ISR(RTC_PIT_vect){
	RTC.PITINTFLAGS = 0x1; //clear interrupt flag
	//PORTA.OUTTGL = LEDPIN;
	uint16_t Pangle = readPos(PAN);
	uint16_t Tiltangle = readPos(TILT);
	int32_t panpidval = PID_compute(&panPID, (int) Pangle);
	int32_t tiltpidval = PID_compute(&tiltPID, (int) Tiltangle);
	//PIT_index++;
	setPanSpeed(panpidval, 0);
	setTiltSpeed(tiltpidval, 1);
	if(PIT_index == 0xF) {
		PIT_index = 0;
		intToHexChar(intmsg, Tiltangle);
		intmsg[0] = 'A';
		USART0_sendString(intmsg, 9);
		intToHexChar(intmsg, tiltpidval);
		intmsg[0] = 'P';
		USART0_sendString(intmsg, 9);
	}
	
}
//usart0 receive complete
ISR(USART0_RCX_vect){
}
