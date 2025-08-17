#include "upperservo.h"
#include "Caculate.h"
#include "param.h"
#include "DJI.h"
#include "decode.h"
#include "cmsis_os.h"
#include "wtr_can.h"
#include "mi_motor.h"
#include "math.h"
float Lcompensation = 1.0;

gantrystate mygantry; 
float littlenum = 0.034;
float largenum = 3.1415926535 * 4.0 / 180.0;
float degreespeed = 80 ;

uint16_t timeout = 0;
void upperservotask(void const * argument)
{
  /* USER CODE BEGIN upperservotask */
  mygantry.gantrypos.x = 2000.0;
  mygantry.gantrypos.y = 720.0;
  Lidar1.distance_aver = 2000.0;
  Lidar6.distance_aver = 2000.0;
  Lidar2.distance_aver = 720;
  osDelay(1800);
  /* Infinite loop */
  for(;;)
  {

    STP_23L_Decode(Rxbuffer_1, &Lidar1);//激光是轴的
    STP_23L_Decode(Rxbuffer_2, &Lidar2);//激光是长轴的
    STP_23L_Decode(Rxbuffer_6, &Lidar6);
    //计算
    // if(fabs(mygantry.gantrypos.degree - mi_motor[0].Angle) > largenum)
    //  motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, MIkp, MIkd);
    // if(fabs(mygantry.gantrypos.degree - mi_motor[0].Angle) > littlenum && fabs(mygantry.gantrypos.degree - mi_motor[0].Angle) < largenum)
    //  motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, 50, 2.0);
    // if(fabs(mygantry.gantrypos.degree - mi_motor[0].Angle) < littlenum)
    //  motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, 20, 1.0);
    // if(fabs(mygantry.gantrypos.degree - mi_motor[0].Angle) > largenum )

    //  motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, MIkp, MIkd);
    if(fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) > 10.0)
      motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, MIkp, MIkd);
    if(fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) > 8.0 && fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) < 10.0)
      motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, 5.0, 1.5);
      if(fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) > 5.0 && fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) < 8.0)
      motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, 10.0, 2.0);
      if(fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) > 3.0 && fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) < 5.0)
      motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, 15.0, 2.5);
    if(fabs(mygantry.gantrypos.degree /3.1415926  * 180.0- mi_motor[0].Angle) < 3.0)
      motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, 20.0, 3.0);
    
      // MIkp = 1.2 ;
      // MIkd = 0.8 ;

      // MIkp = 1.0 ;
      // MIkd = 0.5 ;

      // MIkp = 0.8 ;
      // MIkd = 0.8 ;


    //synchronizedPositionServo(mygantry.gantrypos.x, mygantry.Motor_XL, mygantry.Motor_XR,&Lidar1,Lcompensation, 1.0, -1, 1);
    positionServo_lidar(mygantry.gantrypos.x - 30.0 ,mygantry.Motor_XL, Lidar6);// x轴左
    positionServo_lidar(mygantry.gantrypos.x ,mygantry.Motor_XR, Lidar1);//x轴右
    positionServo_lidar(mygantry.gantrypos.y ,mygantry.Motor_Y, Lidar2);//y轴宽
    positionServo(mygantry.gantrypos.z, mygantry.Motor_Z);
  
    CanTransmit_DJI_1234(&hcan1,
                             -1 * mygantry.Motor_Y->speedPID.output,
                             mygantry.Motor_Y->speedPID.output,
                             mygantry.Motor_XL->speedPID.output,
                             mygantry.Motor_XR->speedPID.output);
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
    mygantry.Motor_XL  = &hDJI[2];
    mygantry.Motor_XR  = &hDJI[3];
    mygantry.Motor_Y = &hDJI[1];
    mygantry.Motor_Z = &hDJI[4];
    //mygantry.Motor_S = &hDJI[5];

    hDJI[1].motorType  = M2006;//y
    hDJI[2].motorType  = M3508;
    hDJI[3].motorType  = M3508;//x
    hDJI[4].motorType  = M3508;//z
    //hDJI[5].motorType  = M2006;//degree

    DJI_Init();

    //pid_reset(&(mygantry.Motor_X->speedPID), 1.2, 0.01, 0.01);//x  //10.0 0.5 0.05
    //pid_reset(&(mygantry.Motor_X->speedPID), 1.1, 0.06, 0.01);//一个箱子
    //pid_reset(&(mygantry.Motor_X->speedPID), 1.0, 0.01, 0.01);//两个箱子

    pid_reset(&(mygantry.Motor_XR->speedPID), 12, 0.2, 5.0);//x  //10.0 0.5 0.05
    pid_reset(&(mygantry.Motor_XR->posPID), 8.0, 0.0, 0.0);//x  //70 0.04 

    pid_reset(&(mygantry.Motor_XL->speedPID), 12, 0.2, 5.0);//x  //10.0 0.5 0.05
    pid_reset(&(mygantry.Motor_XL->posPID), -8.0, 0.0, 0.0);//x  //70 0.04 
    //pid_reset(&(mygantry.Motor_X->posPID), 8.000001, 0.0, 0.0);
    //pid_reset(&(mygantry.Motor_X->posPID), 8.000002, 0.0, 0.0);

    pid_reset(&(mygantry.Motor_Y->speedPID), 12, 0.2, 5.0);

    pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);//y
    //pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);//y
    //pid_reset(&(mygantry.Motor_Y->posPID), 15.000002, 0.0, 0);//y


    pid_reset(&(mygantry.Motor_Z->posPID), 12.0, 0.0, 0);//z

    mygantry.Motor_XL->speedPID.outputMax = 20000;
    mygantry.Motor_XR->speedPID.outputMax = 20000;
    mygantry.Motor_Y->speedPID.outputMax = 20000;
    mygantry.Motor_Z->speedPID.outputMax = 10000;
    //mygantry.Motor_S->speedPID.outputMax = 8000;

    mygantry.Motor_XL->posPID.outputMax = 50000;
    mygantry.Motor_XR->posPID.outputMax = 50000;
    mygantry.Motor_Y->posPID.outputMax = 50000;
    mygantry.Motor_Z->posPID.outputMax = 50000;
    //mygantry.Motor_S->speedPID.outputMax = 8000;

    CANFilterInit(&hcan1);
    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 14;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    can_filter_st.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

    //小米电机调0
    osDelay(100);
    init_cybergear(&mi_motor[0], 0x7F, Motion_mode);
    osDelay(500);
    set_zeropos_cybergear(&mi_motor[0]);
    // Set_Motor_Parameter(&mi_motor[0],0x2013,1.0,'f');
    //Set_Motor_Parameter(&mi_motor[0],0x2013,0.5,'f');
    motor_controlmode(&mi_motor[0], 0, 0, 0, 1.0, 0.5);
    osDelay(20);
    motor_controlmode(&mi_motor[0], 0, 0, 0, 1.0, 0.5);
    osDelay(500);
    //set_position_target_and_speed(&mi_motor[0] , 3.141592654 * 1.0, 80 / 180.0 * 3.1415926);
    // 两个箱子40rad/s  一个箱子60rad/s  空载80rad/s
    //运控模式
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

