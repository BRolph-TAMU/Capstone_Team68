#include <avr/io.h>
#include "timers.h"

void TC_init(void) {
	PORTMUX.TCAROUTEA = 0b11111; //tca0 output on PB4,PB5
	//PORTB.DIRSET = PIN4_bm | PIN5_bm;
	TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
	TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP1EN_bm | TCA_SPLIT_LCMP2EN_bm; //PAN | TILT pins
	
	TCA0.SPLIT.LPER = 0xFF;
	TCA0.SPLIT.HPER = 0xFF;
	//TCA0.SPLIT.LCMP0 = 0x80;
	TCA0.SPLIT.LCMP1 = 0x00; //PAN
	TCA0.SPLIT.LCMP2 = 0x00; //TILT
	TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV16_gc | TCA_SPLIT_ENABLE_bm;
}

void RTC_init(void) {
	RTC.PITCTRLA = RTC_PERIOD_CYC32_gc //1 kHz
	| RTC_PITEN_bm; //enable periodic interrupt timer
	RTC.PITINTCTRL = RTC_PI_bm; //enable interrupt
}
                         
