#include "cmsis_os.h"
#include "upperservo.h"
#include "Caculate.h"
#include "param.h"
#include "DJI.h"
#include "decode.h"
#include "wtr_can.h"


void upeeeMItask(void const * argument)
{
    osDelay(2000);
  /* USER CODE BEGIN upeeeMItask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END upeeeMItask */
}