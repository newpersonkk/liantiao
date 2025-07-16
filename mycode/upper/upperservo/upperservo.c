#include "upperservo.h"
#include "Caculate.h"
#include "param.h"
#include "DJI.h"
#include "decode.h"
#include "cmsis_os.h"
#include "wtr_can.h"
#include "motor.h"

gantrystate mygantry; 


void upperservotask(void const * argument)
{
  /* USER CODE BEGIN upperservotask */
  mygantry.gantrypos.x = 1850.0;
  mygantry.gantrypos.y = 720.0;
  Lidar1.distance_aver = 1855.0;
  Lidar2.distance_aver = 720;
  osDelay(1800);
  /* Infinite loop */
  for(;;)
  {

    STP_23L_Decode(Rxbuffer_1, &Lidar1);//激光是轴的
    STP_23L_Decode(Rxbuffer_2, &Lidar2);//激光是长轴的
    //计算
    positionServo_lidar(mygantry.gantrypos.x, mygantry.Motor_X, Lidar1);//x轴长
    positionServo_lidar(mygantry.gantrypos.y ,mygantry.Motor_Y, Lidar2);//y轴宽

    positionServo(mygantry.gantrypos.z, mygantry.Motor_Z);
    //positionServo(mygantry.gantrypos.degree, mygantry.Motor_S);
  
    CanTransmit_DJI_1234(&hcan1,
                             -1 * mygantry.Motor_Y->speedPID.output,
                             mygantry.Motor_Y->speedPID.output,
                            -1 * mygantry.Motor_X->speedPID.output,
                             mygantry.Motor_X->speedPID.output);
    
    CanTransmit_DJI_5678(&hcan1,
                             mygantry.Motor_Z->speedPID.output ,
                             0 ,
                             0,
                             0);
    
    osDelay(1);

  }
  /* USER CODE END upperservotask */
}
void gantry_Motor_init()               //电机初始化
{
    
    mygantry.Motor_X  = &hDJI[3];
    mygantry.Motor_Y = &hDJI[1];
    mygantry.Motor_Z = &hDJI[4];
    //mygantry.Motor_S = &hDJI[5];

    hDJI[1].motorType  = M2006;//y
    hDJI[3].motorType  = M3508;//x
    hDJI[4].motorType  = M3508;//z
    //hDJI[5].motorType  = M2006;//degree

    DJI_Init();

    pid_reset(&(mygantry.Motor_X->speedPID), 1.8, 0.01, 0.001);//x  //10.0 0.5 0.05
    //pid_reset(&(mygantry.Motor_X->speedPID), 1.8, 0.003, 0.001);
    //pid_reset(&(mygantry.Motor_X->speedPID), 1.2, 0.0015, 0.001);

    pid_reset(&(mygantry.Motor_X->posPID), 6.0, 0.0, 0.0);//x  //70 0.04 

    pid_reset(&(mygantry.Motor_Y->speedPID), 1.5, 0.01, 0.01);
    //pid_reset(&(mygantry.Motor_Y->speedPID), 1.3, 0.015, 0.01);
    //pid_reset(&(mygantry.Motor_Y->speedPID), 1.3, 0.018, 0.02);

    pid_reset(&(mygantry.Motor_Y->speedPID), 10.0, 1.0, 2.0);//y
    pid_reset(&(mygantry.Motor_Y->posPID), 10.0, 0.0, 0);//y


    pid_reset(&(mygantry.Motor_Z->posPID), 8.0, 0.0, 0);//z
    
    //pid_reset(&(mygantry.Motor_S->speedPID), 12.0, 0.5, 5.0);//x  //  12.0  0.5  5.0空转    12.0 0.12 4.0一个箱子  //12.0 0.02 2.0 两个箱子
    //pid_reset(&(mygantry.Motor_S->speedPID), 12.0, 0.12, 4.0);
    //pid_reset(&(mygantry.Motor_S->speedPID), 12.0, 0.032, 2.5); 
    //pid_reset(&(mygantry.Motor_S->posPID),15.0, 0.0, 0);//z   15空转  2.0一个箱子 1.5两个箱子


    mygantry.Motor_X->speedPID.outputMax = 10000;
    mygantry.Motor_Y->speedPID.outputMax = 10000;
    mygantry.Motor_Z->speedPID.outputMax = 10000;
    //mygantry.Motor_S->speedPID.outputMax = 8000;

    mygantry.Motor_X->posPID.outputMax = 20000;
    mygantry.Motor_Y->posPID.outputMax = 20000;
    mygantry.Motor_Z->posPID.outputMax = 20000;
    //mygantry.Motor_S->speedPID.outputMax = 8000;

    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    can_filter_st.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

    //小米电机调0
    osDelay(100);
    init_cybergear(&mi_motor[0], 0x7F, Motion_mode);
    set_zeropos_cybergear(&mi_motor[0]);
    motor_controlmode(&mi_motor[0], 0, 0, 0, 1, 0.5);
    osDelay(100);
    motor_controlmode(&mi_motor[0], 0, 0, 0, 1, 0.5);

    CANFilterInit(&hcan1);

    //小米电机 过滤器设置 hcan2

    //逆时针旋转为正
    //motor_controlmode(&mi_motor[0], 0, 3.1415926/2.0f, 0, 1.0, 0.5);
    //小米电机夹住�?个箱�?
    //motor_controlmode(&mi_motor[0], 0, 3.1415926*3.0/2.0f, 0, 1.8, 1.0);
    //小米电机夹住两个箱子
    //motor_controlmode(&mi_motor[0], 0, 3.1415926*3.0/2.0f, 0, 2.2, 1.5);
}

void pid_reset(PID_t *pid, float kp, float ki, float kd)
{
    pid->KP       = kp;
    pid->KI       = ki;
    pid->KD       = kd;
}

