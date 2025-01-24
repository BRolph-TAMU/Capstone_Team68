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
	pid->minspeed = 0;
}

int32_t PID_compute(PID* pid, int16_t actual){
	//error range 4096 to -4096; 13 bit                                                                                  
	int16_t error = actual - pid->setpoint;
	if (error > 2048) {error -= 4096;} //make the 0-4096 scale wrap around
	if (error < -2048) {error += 4096;} //more smoothly
	if (abs(error) <= 5) {return 0;}
	
	//Pout range 21 bit
	int32_t Pout = pid->kp * (int32_t)error;
	
	pid->integral += (error>>1);
	if ((abs(error) > 0x200)) {pid->integral = 0;}

	if (pid->integral > 65535) {pid->integral = 65535;}
	if (pid->integral < -65535) {pid->integral = -65535;}
	int32_t Iout = ((pid->integral)>>4) * pid->ki;
	
	int32_t Dout = (error - pid->prev_err) * pid->kd;
	
	int32_t out = Pout + Dout + Iout;
	pid->prev_err = error;
	if (out > 8388607) {out = 8388607;} //2^23 - 1
	if (out < -8388608) {out = -8388608;} //-2^23
	if (out == 0) {return 0;}
	if (out > 0) return (out >> 6) + (pid->minspeed)-0xF;
	return (out >> 6) - (pid->minspeed)+0xF;
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
  val &= 0xFFF; //limit to 12 bit only
  if (axis == PAN){panPID.setpoint = val;}
  if (axis == TILT){tiltPID.setpoint = val;}
}



