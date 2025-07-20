#ifndef _CACULATE_H__
#define _CACULATE_H__
#include "decode.h"
#include "DJI.h"



void positionServo(float ref, DJI_t * motor);
void positionServo_lidar(float ref, DJI_t *motor, LidarPointTypedef lidardata);
void speedServo(float ref, DJI_t * motor);
void synchronizedPositionServo(float ref, DJI_t *leftMotor, DJI_t *rightMotor, 
                              LidarPointTypedef *lidarData, 
                              float leftCompensation, float rightCompensation,
                              int leftDirection, int rightDirection);

#endif