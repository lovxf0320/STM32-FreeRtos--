#ifndef __USART_H
#define __USART_H
//#include "sys.h"
#include "stm32f10x.h"

#define USART2_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART2_RX_EN 			1					//0,不接收;1,接收.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART2_RX_STA;   						//接收数据状态

extern u8 WF_Data1;

void USART2_WIFI_CTRL(void);
void usart2_init(u32 bound);				//串口2初始化 
void u2_printf(char* fmt,...);
#endif