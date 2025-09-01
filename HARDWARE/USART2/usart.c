#include "delay.h"
#include "usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "lcd.h"
#include "my_task.h"

//串口接收缓冲区
extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节

// 接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14]:接收到的数据长度
extern vu16 USART2_RX_STA=0;   						//接收数据状态
u8 i=0;
u8 WF_Data1=0;

void USART1_IRQHandler(void){
    u8 res;

    //FreeRTOS相关标志，用于判断是否切换任务
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken=pdFALSE;               //初始化为FALSE

    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){     //检查USART_IT_RXNE这个寄存器是否被占用
        res=USART_ReceiveData(USART2);              //从USART2读取一个字节
        USART_SendData(DEBUG_USARTx,res);           //转发到调试串口

        //15位为1：完成，15位为0：未完成
        if(res!='A'){                               //以A为结束符，不是结束符'A'
            if((USART2_RX_STA)&(1<<15)==0){         //如果当前消息未标记为完成（bit 15 为 0），则继续接收数据并存入缓冲区
                USART2_RX_BUF[USART2_RX_STA++]=res; //记录接收到的值
            }else{
                USART2_RX_STA|=1<<15;               //如果收到 'A'，直接将 USART2_RX_STA 的 bit 15 置为 1，标记消息接收完成
            }
        }else{
            USART2_RX_STA|=1<<15;                   //接收消息完成
        }

        if(BinarySemaphore!=NULL){
            i++;
            if(i==2){
                xSemaphoreGiveFromISR(BinarySemaphore,&xHigherPriorityTaskWoken);
                portYIELD_FORM_ISR(xHigherPriorityTaskWoken);
                //printf("释放信号量");
                i=0;
            }
        }
    }
}


void USART2_WIFI_CLRT(void){
    if(USART2_RX_STA & 0X8000){     //如果消息接收完成，则处理
        if(strstr((const char*)USART2_RX_BUF,"stop")==1){
            GPIO_ResetBits(GPIOA,GPIO_Pin_8);
            WF_Data1=0X01;
        }

        if(strstr((const char*)USART2_RX_BUF,"go")==1){
            GPIO_SetBits(GPIOA,GPIO_Pin_8);
            WF_Data1=0X02;
        }

        if(strstr((const char*)USART2_RX_BUF,"back")==1){
            GPIO_SetBits(GPIOA,GPIO_Pin_8);
            WF_Data1=0X03;
        }

        if(strstr((const char*)USART2_RX_BUF,"left")==1){
            WF_Data1=0X04;
        }

        if(strstr((const char*)USART2_RX_BUF,"right")==1){
            WF_Data1=0X05;
        }

        USART2_RX_STA=0;                //消息处理完成，开始重新接收
        memset(USART2_RX_BUF,0,600);    //将接收区清0
    }
}


void usart2_init(u32 bound){
    //1.定义结构体变量
    NAIC_InitTypeDef NVIC_InitStructure;      //中断控制器配置结构体
    GPIO_InitTypeDef GPIO_InitStructure;      //GPIO配置结构体
    USART_InitTypeDef USART_InitStructure;    //串口配置结构体

    //2.开启相关时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);   //开启USART2时钟

    //3.复位串口
    USART_DeInit(USART2);

    //4.配置GPIO引脚
    //配置TX引脚(PA2)
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;         //选择PA2引脚
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHZ; //设置输出速度50MHz
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   //复用推挽输出模式
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    //配置RX引脚
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;              //选择PA3引脚
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOAING;   //浮空输入模式
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    //5.配置串口参数
    USART_InitStructure.USART_BaudRate = bound;                   // 波特率（函数参数）
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;   // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;        // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;           // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式都开启
    USART_Init(USART2,&USART_InitStructure);    //应用串口配置
    USART_Cmd(USART2,ENABLE);                   //使能串口

    //6.配置接收中断

    //开启接收中断
    //USART2：选择串口2
    //USART_IT_RXNE：接收数据寄存器非空中断
    //ENABLE：使能这个中断
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

    //配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQ;          //中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5; //抢占优先级5
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;        //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //使能中断
    NVIC_Init(&NVIC_InitStructure);                         //应用中断配置

    //7.清零状态变量
    USART2_RX_STA=0;                //清零接收状态标志
}

//串口2 printf封装
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char *fmt,...){
    u16 i,j;        //16位无符号整数，i存储字符串长度，j作为循环计数器
    va_list ap;     //可变参数列表指针

    va_start(ap,fmt);   //初始化可变参数列表
    vsprintf((char *)USART2_TX_BUF, fmt, ap);  // 将格式化后的字符串存入缓冲区
    va_end(ap);         //结束可变参数处理

    i = strlen((const char *)USART2_TX_BUF);  // 获取要发送的字符串长度
    for (j = 0; j < i; j++) {  // 逐字节发送
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  //等待发送完成
        USART_SendData(USART2, USART2_TX_BUF[j]);  //发送一个字节
    }
}

