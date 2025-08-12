
#include "usart.h"
#include "stm32f4xx_it.h"
#include "decode.h"
#include "string.h"
#include "param.h"

static uint16_t u4state = 0;
static uint16_t u5state = 0;

uint16_t flag = 0;

uint16_t rx_complete1 = 0;
uint16_t rx_complete2 = 0;

uint8_t uart4_rx[1];
uint8_t uart5_rx[1];

static uint8_t rx_buffer4[6];   // 接收缓冲区
static uint8_t rx_buffer5[6];   // 接收缓冲区

char tx_buffer4[6]; 
char tx_buffer5[6]; 

uint8_t Rxbuffer_1[195];
uint8_t Rxbuffer_2[195];
uint8_t Rxbuffer_6[195];

uint16_t UartFlag[6];

LidarPointTypedef Lidar1;
LidarPointTypedef Lidar2;
LidarPointTypedef Lidar6;

uint8_t usart1_rx[1];
uint8_t usart2_rx[1];
uint8_t usart6_rx[1];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        static uint16_t u1state = 0; //状态机计数
        static uint16_t crc1    = 0; //校验和
        uint8_t tmp1   = usart1_rx[0];
        if (u1state < 4)
        {
            if (tmp1 == 0xAA)
            {
                Rxbuffer_1[u1state] = tmp1;
                u1state++;            
            } else {
                u1state = 0;
            }
        } else if (u1state < 194) {
            Rxbuffer_1[u1state] = tmp1;
            u1state++;
            crc1 += tmp1;
        }else if(u1state==194){
            Rxbuffer_1[u1state] = tmp1;
            if (tmp1 == crc1 % 256) 
            {
                UartFlag[0] = 1;
            }
            u1state = 0;
            crc1    = 0;
        } else {
        };

        HAL_UART_Receive_IT(&huart1, usart1_rx, 1);
    }
    if (huart->Instance == USART2) {
        static uint16_t u2state = 0; // 状态机计数
        static uint16_t crc2    = 0; // 校验和
        uint8_t tmp2 = usart2_rx[0];
        if (u2state < 4) {
            if (tmp2 == 0xAA) {
                Rxbuffer_2[u2state] = tmp2;
                u2state++;

            } else {
                u2state = 0;
            }
        } else if (u2state < 194) {
            Rxbuffer_2[u2state] = tmp2;
            u2state++;
            crc2 += tmp2;
        } else if (u2state == 194) {
            Rxbuffer_2[u2state] = tmp2;
            if (tmp2 == crc2 % 256) {
                UartFlag[1] = 1;
            }
            u2state = 0;
            crc2    = 0;
        } else {
        };
        HAL_UART_Receive_IT(&huart2, usart2_rx, 1);
    }
    if (huart->Instance == UART4)
    {
      rxIndex1++;
    if (rxIndex1 >= BUFFER_SIZE) {
      ProcessData1();
      rxIndex1 = 0; // 重置缓冲区
    }
    // 继续接收下一个字节
    HAL_UART_Receive_IT(&huart4, &rxBuffer1[rxIndex1], 1);
  }
    if (huart->Instance == UART5)
    {
         flag = 1;
      rxIndex2++;
    if (rxIndex2 >= BUFFER_SIZE) {
      ProcessData2();
      rxIndex2 = 0; // 重置缓冲区
    }
 
    // 继续接收下一个字节
    HAL_UART_Receive_IT(&huart5, &rxBuffer2[rxIndex2], 1);
  }
  if (huart->Instance == USART6) {
        static uint16_t u6state = 0; // 状态机计数
        static uint16_t crc6    = 0; // 校验和
        uint8_t tmp6            = usart6_rx[0];
        if (u6state < 4) {
            if (tmp6 == 0xAA) {
                Rxbuffer_6[u6state] = tmp6;
                u6state++;

            } else {
                u6state = 0;
            }
        } else if (u6state < 194) {
            Rxbuffer_6[u6state] = tmp6;
            u6state++;
            crc6 += tmp6;
        } else if (u6state == 194) {
            Rxbuffer_6[u6state] = tmp6;
            if (tmp6 == crc6 % 256) {
                UartFlag[3] = 1;
            }
            u6state = 0;
            crc6    = 0;
        } else {
        };

        HAL_UART_Receive_IT(&huart6, usart6_rx, 1);
    }
}