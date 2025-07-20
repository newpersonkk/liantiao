
#ifndef __PARAM_H__
#define __PARAM_H__
#include "decode.h"
#include "DJI.h"
typedef struct 
{
    double x;
    double y;
    double z;
    double degree;
}gantryposition;

typedef struct
{
    DJI_t *Motor_XL;
    DJI_t *Motor_XR;
    DJI_t *Motor_Y;
    DJI_t *Motor_Z;
    DJI_t *Motor_S;
    gantryposition gantrypos;
}
gantrystate;


extern int16_t xiangzi[6]; 
extern int16_t zhiduo[6];  

extern gantrystate mygantry; 

extern uint8_t usart1_rx[1];
extern uint8_t usart2_rx[1];
extern uint8_t usart6_rx[1];
extern uint8_t uart4_rx[1];
extern uint8_t uart5_rx[1];

extern char tx_buffer4[6]; 
extern char tx_buffer5[6]; 

extern uint8_t Rxbuffer_1[195];
extern uint8_t Rxbuffer_2[195];
extern uint8_t Rxbuffer_6[195];

extern LidarPointTypedef Lidar1;
extern LidarPointTypedef Lidar2;

extern uint16_t jiaquzhuangtai ;
#endif // !__PARAM_H__PARAM_H__