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

PID panPID; //create pan axis pid obect
PID tiltPID; //tilt axis
char message[5] = {'\n','\n','\n','\n','\n'}; //these are all message buffers
char pidmsg[5] = {'p', 'i', 'd',':',' '};
char angmsg[5] = {'a', 'n', 'g',':',' '};
char intmsg[9] = {'\n','\n','\n','\n','\n','\n','\n','\n','\n'};
//int16_t lastval = 0;
//uint8_t PIT_index = 0;
int main(){
	sei(); //enable interrupts
	inits(); //initialization functions
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); //No prescaler, 20 MHz clock
	uint16_t commandReturn = 0; //initialize command return code
	PID_init(&panPID, 0x80, 0, 0, 0x400); //inititialize pan axis
	PID_init(&tiltPID, 0x80, 0, 0, 0xF00); //tilt axis
	delayms(100); //let things settle down while peripherals initialize
	USART0_init(); //init usart module
	PORTA.OUTCLR = LEDPIN; //turn off indicator led
	panPID.minspeed = findMinSpeed(PAN); //functions to find minimum speed value
	//panPID.minspeed = 0x20;
	//tiltPID.minspeed = 0x20;
	tiltPID.minspeed = findMinSpeed(TILT);
	RTC_init(); //wait til now to start so that it doesnt interrupt findminspeed
	delayms(100); //some more settling
	USART0_flush(); //flush any junk in usart buffer
	while(1){ //infinite loop
		commandReturn = getCommand(); //command return code
		intToHexChar(intmsg, commandReturn); //convert code to ascii
		intmsg[0] = 'M'; //prefix 'M' = return code
		USART0_sendString(intmsg, 9); //send to host pc
	}

	return 0;
}



//periodic interrupt timer ISR.
ISR(RTC_PIT_vect){
	RTC.PITINTFLAGS = 0x1; //clear interrupt flag
	uint16_t Pangle = readPos(PAN); //find pan axis angle
	uint16_t Tiltangle = readPos(TILT); //tilt axis angle
	int32_t panpidval = PID_compute(&panPID, (int) Pangle); //compute pan pid
	int32_t tiltpidval = PID_compute(&tiltPID, (int) Tiltangle); //compute tilt pid
	setPanSpeed(panpidval, 0); //set pan speed
	setTiltSpeed(tiltpidval, 1); //set tilt speed
	
}
//usart0 receive complete
ISR(USART0_RCX_vect){
}
