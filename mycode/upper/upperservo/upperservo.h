#ifndef __UPPERSERVO_H__
#define __UPPERSERVO_H__

#include "DJI.h"

void upperservotask(void const * argument);
void gantry_Motor_init();
void pid_reset(PID_t *pid, float kp, float ki, float kd);


#endif
