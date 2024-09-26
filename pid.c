#include <avr/io.h>
#include "pid.h"

void PID_init(PIDctrl *pid, uint8_t kp, uint8_t ki, uint8_t kd, int16_t setpoint){
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->setpoint = setpoint;
	pid->prev_err = 0;
	pid->integral = 0;
}

uint16_t PID_compute(PIDCTRL *pid, int16_t actual){
	
	int16_t error = actual - pid->setpoint;
	
	int32_t Pout = pid->kp * error;
	
	pid->integral += (error>>2);
	if (pid->integral > (INT32_MAX >> 1) ) {
		pid->integral
	
	int32_t Dout = (error - pid->prev_err) * pid->kd;
	
	int32_t out = Pout + Dout;
	
