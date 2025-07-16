/*过滤器配置，can的fifo0接收*/

#include "wtr_can.h"
#include "motor.h"
uint8_t CanReceiveData[8];


HAL_StatusTypeDef CANFilterInit(CAN_HandleTypeDef* hcan){
  CAN_FilterTypeDef  sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;
  
	
  if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(hcan) != HAL_OK)
  {
    Error_Handler();
  }
	
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

	return HAL_OK;
}

void CanDataDecode(CAN_RxHeaderTypeDef RxHeader){
  /* Can message Decode */
  if( RxHeader.IDE == CAN_ID_STD ){
    DJI_CanMsgDecode(RxHeader.StdId, CanReceiveData);
  }
  if( RxHeader.IDE == CAN_ID_EXT ){
    // vesc反馈关掉这里就不会有消息
    ;;
  }
  
}

/*
HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxMsg, rx_data);//接收数据
	  Motor_Can_ID=Get_Motor_ID(rxMsg.ExtId);//首先获取回传电机ID信息  
    switch(Motor_Can_ID)                   //将对应ID电机信息提取至对应结构体
    {
        case 0X7F:  
            if(rxMsg.ExtId>>24 != 0)               //检查是否为广播模式
                Motor_Data_Handler(&mi_motor[0],rx_data,rxMsg.ExtId);
            else 
                mi_motor[0].MCU_ID = rx_data[0];
            break;           
        default:
            break;		
    }
*/

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
    CAN_RxHeaderTypeDef   RxHeader;
    if( hcan->Instance == hcan1.Instance ){
        if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CanReceiveData) != HAL_OK)
        {

            Error_Handler();
        }
        CanDataDecode(RxHeader);
    }
    if( hcan->Instance == hcan2.Instance ){
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxMsg, rx_data);//接收数据
	  Motor_Can_ID=Get_Motor_ID(rxMsg.ExtId);//首先获取回传电机ID信息  
    switch(Motor_Can_ID)                   //将对应ID电机信息提取至对应结构体
    {
        case 0X7F:  
            if(rxMsg.ExtId>>24 != 0)               //检查是否为广播模式
                Motor_Data_Handler(&mi_motor[0],rx_data,rxMsg.ExtId);
            else 
                mi_motor[0].MCU_ID = rx_data[0];
            break;           
        default:
            break;		
    }
    }


}
