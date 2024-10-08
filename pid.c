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

int32_t PID_compute(PID* pid, int16_t actual){
	//error range 4096 to -4096; 13 bit                                                                                  
	int16_t error = actual - pid->setpoint;
	//Pout range 21 bit
	int32_t Pout = pid->kp * (int32_t)error;
	
	if ((abs(error) > 10)) {pid->integral += error>>3;}

	if (pid->integral > 4096) {pid->integral = 4096;}
	if (pid->integral < -4096) {pid->integral = -4096;}
	int32_t Iout = pid->integral * pid->ki;
	
	int32_t Dout = (error - pid->prev_err) * pid->kd;
	
	int32_t out = Pout + Dout + Iout;
	pid->prev_err = error;
	if (out > 8388607) {out = 8388607;} //2^23 - 1
	if (out < -8388608) {out = -8388608;} //-2^23
	return out >> 8;
}
	
void setProportional(uint8_t axis, uint16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.kp = val;}
  if (axis == TILT){tiltPID.kp = val;}
}

void setIntegral(uint8_t axis, uint16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.ki = val;}
  if (axis == TILT){tiltPID.ki = val;}
}
void setDerivative(uint8_t axis, uint16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.kd = val;}
  if (axis == TILT){tiltPID.kd = val;}
}

void setPoint(uint8_t axis, int16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.setpoint = val >> 4;}
  if (axis == TILT){tiltPID.setpoint = val;}
}

