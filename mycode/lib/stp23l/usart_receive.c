
#include "usart.h"
#include "stm32f4xx_it.h"
#include "decode.h"
#include "string.h"


static uint16_t u4state = 0;
static uint16_t u5state = 0;

uint8_t uart4_rx[1];
uint8_t uart5_rx[1];

static uint8_t rx_buffer4[6];   // 接收缓冲区
static uint8_t rx_buffer5[6];   // 接收缓冲区

char tx_buffer4[6]; 
char tx_buffer5[6]; 

uint8_t Rxbuffer_1[195];
uint8_t Rxbuffer_2[195];

uint16_t UartFlag[6];

LidarPointTypedef Lidar1;
LidarPointTypedef Lidar2;

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
        rx_buffer4[u4state] = uart4_rx[1]; // 存入缓冲区
        u4state++;
        if (u4state >= 6)
        {
            memcpy(tx_buffer4, rx_buffer4, 6);// 复制到目标缓冲区rx_complete=1; // 设置完成标志
        }
        else 
        {
            HAL_UART_Receive_IT(&huart4, uart4_rx, 1);
        }
    }
    if (huart->Instance == UART5)
    {
        rx_buffer5[u5state] = uart5_rx[1]; // 存入缓冲区
        u5state++;
        if (u5state >= 6)
        {
            memcpy(tx_buffer5, rx_buffer5, 6);// 复制到目标缓冲区rx_complete=1; // 设置完成标志
        }
        else 
        {
            HAL_UART_Receive_IT(&huart5, uart5_rx, 1);
        }
    }
}