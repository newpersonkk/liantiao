#ifndef __UPPERRUN_H__
#define __UPPERRUN_H__
#include "stm32f4xx_it.h"
#include "cmsis_os.h"
void uppergoingtask(void const * argument);
void matchingnum(int16_t arr1[], const char arr2[]);

#endif // !__UPPERRUN_H__