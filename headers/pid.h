#ifndef PID_H
#define PID_H

typedef struct {
	int16_t setpoint;
	uint8_t kp;
	uint8_t ki;
	uint8_t kd;
	int16_t prev_err;
	int32_t integral;
} PID;

void PID_init(PID* pid, uint8_t kp, uint8_t ki, uint8_t kd, int16_t setpoint);

int32_t PID_compute(PID* pid, int16_t actual);

extern PID panPID;
extern PID tiltPID;

void setProportional(uint8_t axis, uint16_t val);
void setDerivative(uint8_t axis, uint16_t val);
void setIntegral(uint8_t axis, uint16_t val);
void setPoint(uint8_t axis, int16_t val);
#endif
