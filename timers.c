#include <avr/io.h>
#include "timers.h"

void TC_init(void) {
	;
}

void RTC_init(void) {
	RTC.PITCTRLA = RTC_PERIOD_CYC512_gc //64 Hz
	| RTC_PITEN_bm; //enable periodic interrupt timer
	RTC.PITINTCTRL = RTC_PI_bm; //enable interrupt
}
