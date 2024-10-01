#include <avr/io.h>
#include "pid.h"
#include "globals.h"

void PID_init(PID *pid, uint8_t kp, uint8_t ki, uint8_t kd, int16_t setpoint){
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->setpoint = setpoint;
	pid->prev_err = 0;
	pid->integral = 0;
}

int16_t PID_compute(PID* pid, int16_t actual){
	//error range 4096 to -4096; 13 bit                                                                                  
	int16_t error = actual - pid->setpoint;
	//Pout range 21 bit
	int32_t Pout = pid->kp * error;
	
	pid->integral += error>>2;
	if (pid->integral > 65535) {pid->integral = 65535;}
	if (pid->integral < -65535) {pid->integral = -65535;}
	int32_t Iout = integral * pid->ki;
	
	int32_t Dout = (error - pid->prev_err) * pid->kd;
	
	int32_t out = Pout + Dout + Iout;
	pid->prev_err = error;
	if (out > 8388607) {out = 8388607;} //2^23 - 1
	if (out < -8388608) {out = -8388608;} //-2^23
	return out >> 8;
}
	
