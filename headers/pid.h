#ifndef PID_H
#define PID_H

typedef struct {
	int16_t setpoint;
	uint8_t kp;
	uint8_t ki;
	uint8_t kd;
	int16_t prev_err;
	int32_t integral;
} PIDctrl;

void PID_init(PIDctrl *pid, uint8_t kp, uint8_t ki, uint8_t kd, int16_t setpoint);

int16_t PID_compute(PIDCTRL *pid, int16_t actual);
#endif
