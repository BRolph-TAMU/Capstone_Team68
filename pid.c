#include <avr/io.h>
#include "pid.h"
#include "globals.h"

char parammsg[9] = {'\n','\n','\n','\n','\n','\n','\n','\n','\n'};


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
	if (error > 2048) {error -= 4096;}
	if (error < -2048) {error += 4096;}
	if (abs(error) <= 4) {return 0;}
	
	//Pout range 21 bit
	int32_t Pout = pid->kp * (int32_t)error;
	
	if ((abs(error) < 0x100)) {pid->integral += (error>>1);}

	if (pid->integral > 4096) {pid->integral = 4096;}
	if (pid->integral < -4096) {pid->integral = -4096;}
	int32_t Iout = pid->integral * pid->ki;
	
	int32_t Dout = (error - pid->prev_err) * pid->kd;
	
	int32_t out = Pout + Dout + Iout;
	pid->prev_err = error;
	if (out > 8388607) {out = 8388607;} //2^23 - 1
	if (out < -8388608) {out = -8388608;} //-2^23
	if (out == 0) {return 0;}
	if (out > 0) return (out >> 8) + pid->minspeed-0x0;
	return (out >> 8) - pid->minspeed+0x0;
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

void printParams(uint8_t axis){
	if (axis == PAN){
		intToHexChar(parammsg, ((int32_t)(panPID.kp)<<24) + ((int32_t)(panPID.ki)<<12)+(int32_t)(panPID.kd));
		//intmsg[0] = 'M';
		USART0_sendString(parammsg, 9);
	}
	else{
		intToHexChar(parammsg, ((int32_t)(tiltPID.kp)<<24) + ((int32_t)(tiltPID.ki)<<12)+(int32_t)(tiltPID.kd));
		//intmsg[0] = 'M';
		USART0_sendString(parammsg, 9);
	}
}

