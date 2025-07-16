#ifndef __UPEERSTART_H__
#define __UPEERSTART_H__
#include "upperservo.h"
#include "retarget.h"
#include "param.h"
#include "cmsis_os.h"
#include "tim.h"
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  gantry_Motor_init();  

  HAL_UART_Receive_IT(&huart1, usart1_rx, 1);
  HAL_UART_Receive_IT(&huart2, usart2_rx, 1);
  osDelay(1000);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  RetargetInit(&huart3);
  
  /* Infinite loop */
  for(;;)
  {
    printf("%f,%f,%f,%f,%f,%f,%f,%f\r\n",
               Lidar1.distance_aver,
               Lidar2.distance_aver,
               hDJI[4].AxisData.AxisAngle_inDegree,
               hDJI[5].AxisData.AxisAngle_inDegree,
               jiaquzhuangtai,
               mygantry.gantrypos.x,
               mygantry.gantrypos.y,
               mygantry.gantrypos.z
               );
             /*printf("%f,%d,%f,%d\r\n",
               hDJI[5].AxisData.AxisAngle_inDegree,
               jiaquzhuangtai,
               mygantry.Motor_S->speedPID.output,
               Lidar1.distance_aver
               );  */
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}



#endif // !1
