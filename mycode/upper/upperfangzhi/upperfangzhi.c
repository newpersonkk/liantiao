#include "cmsis_os.h"
#include "upperservo.h"
#include "Caculate.h"
#include "param.h"
#include "DJI.h"
#include "decode.h"
#include "wtr_can.h"
#include "motor.h"


void upeeeMItask(void const * argument)
{
    osDelay(2000);
  /* USER CODE BEGIN upeeeMItask */
  /* Infinite loop */
  for(;;)
  {
    motor_controlmode(&mi_motor[0], 0, mygantry.gantrypos.degree, 0, MIkp, MIkd);
    osDelay(5);
  }
  /* USER CODE END upeeeMItask */
}