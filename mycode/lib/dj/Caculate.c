/*pid算法和速度位置伺服*/

#include "Caculate.h"
#include "math.h"
#include "decode.h"
//增量式PID算法
void PID_Calc(PID_t *pid){
	pid->cur_error = pid->ref - pid->fdb;
	pid->output += pid->KP * (pid->cur_error - pid->error[1]) + pid->KI * pid->cur_error + pid->KD * (pid->cur_error - 2 * pid->error[1] + pid->error[0]);
	pid->error[0] = pid->error[1];
	pid->error[1] = pid->ref - pid->fdb;
	/*设定输出上限*/
	if(pid->output > pid->outputMax) pid->output = pid->outputMax;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax;

}

//比例算法
void P_Calc(PID_t *pid){
	pid->cur_error = pid->ref - pid->fdb;
	pid->output = pid->KP * pid->cur_error;
	/*设定输出上限*/
	if(pid->output > pid->outputMax) pid->output = pid->outputMax;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax;
	
	if(fabs(pid->output)<pid->outputMin)
		pid->output=0;

}

void positionServo_lidar(float ref, DJI_t *motor, LidarPointTypedef lidardata)
{

    motor->posPID.ref = ref;
    motor->posPID.fdb = lidardata.distance_aver;
    PID_Calc(&motor->posPID);

    motor->speedPID.ref = motor->posPID.output;
    motor->speedPID.fdb = motor->FdbData.rpm;
    PID_Calc(&(motor->speedPID));
}

//位置伺服函数
void positionServo(float ref, DJI_t * motor){
	
	motor->posPID.ref = ref;
	motor->posPID.fdb = motor->AxisData.AxisAngle_inDegree;
	PID_Calc(&motor->posPID);
	
	motor->speedPID.ref = motor->posPID.output;
	motor->speedPID.fdb = motor->FdbData.rpm;
	PID_Calc(&motor->speedPID);

}

//速度伺服函数
void speedServo(float ref, DJI_t * motor){
	motor->speedPID.ref = ref;
	motor->speedPID.fdb = motor->FdbData.rpm;
	PID_Calc(&motor->speedPID);
}

void synchronizedPositionServo(float ref, DJI_t *leftMotor, DJI_t *rightMotor, 
                              LidarPointTypedef *lidarData, 
                              float leftCompensation, float rightCompensation,
                              int leftDirection, int rightDirection) {
    // 使用同一个Lidar数据作为位置反馈
    float commonPositionFdb = lidarData->distance_aver;
    
    // 计算位置误差并更新位置PID
    leftMotor->posPID.ref = ref;
    leftMotor->posPID.fdb = commonPositionFdb;
    PID_Calc(&leftMotor->posPID);
    
    rightMotor->posPID.ref = ref;
    rightMotor->posPID.fdb = commonPositionFdb;
    PID_Calc(&rightMotor->posPID);
    
    // 应用负载补偿系数和旋转方向，调整左右电机的速度目标
    float leftSpeedRef = leftMotor->posPID.output * leftCompensation * leftDirection;
    float rightSpeedRef = rightMotor->posPID.output * rightCompensation * rightDirection;
    
    // 更新速度PID
    leftMotor->speedPID.ref = leftSpeedRef;
    leftMotor->speedPID.fdb = leftMotor->FdbData.rpm;
    PID_Calc(&leftMotor->speedPID);
    
    rightMotor->speedPID.ref = rightSpeedRef;
    rightMotor->speedPID.fdb = rightMotor->FdbData.rpm;
    PID_Calc(&rightMotor->speedPID);
}

