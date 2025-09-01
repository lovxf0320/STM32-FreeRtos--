#include "sys.h"           // 系统相关定义
#include "usart.h"         // 串口相关定义
#include "string.h"        // 字符串操作函数
#if SYSTEM_SUPPORT_OS      // 如果使用 FreeRTOS
#include "FreeRTOS.h"      // FreeRTOS 核心头文件
#include "task.h"          // FreeRTOS 任务管理
#include "semphr.h"        // FreeRTOS 信号量管理
#endif

// 禁用半主机模式，支持 printf 函数无需 MicroLIB
#if 1
#pragma import(__use_no_semihosting)

// 定义标准输出文件结构体
struct __FILE
{
    int handle; // 文件句柄
};
FILE __stdout;  // 标准输出文件句柄

// 定义 _sys_exit 函数，避免半主机模式调用
void _sys_exit(int x)
{
    x = x; // 空实现
}
#endif

/*使用microLib的方法*/
/* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/


#if EN_USART1_RX // 如果使能串口1接收
// 接收缓冲区，最大长度为 USART_REC_LEN（200 字节）
u8 USART_RX_BUF[USART_REC_LEN];

// 接收状态标志
// bit15：接收完成标志
// bit14：接收到 0x0D（回车符）
// bit13~0：接收到的有效字节数
u16 USART_RX_STA = 0;

// 配置 NVIC 中断优先级
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置优先级分组为 Group 4
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ; // 指定串口中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_Init(&NVIC_InitStructure);
}

// 初始化串口及相关 GPIO 和中断
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 启用串口 GPIO 时钟
    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    // 启用串口外设时钟
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    // 配置 TX 引脚为推挽复用输出
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // 配置 RX 引脚为浮空输入
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // 配置串口参数
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 数据位：8 位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 停止位：1 位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(DEBUG_USARTx, &USART_InitStructure);

    // 配置接收中断
    NVIC_Configuration();
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);

    // 启用串口
    USART_Cmd(DEBUG_USARTx, ENABLE);
}

// 发送单个字节
// 参数:
//   pUSARTx: 串口外设指针（如 USART1）
//   data: 要发送的 8 位数据
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t data)
{
    USART_SendData(pUSARTx, data); // 写入数据到发送寄存器
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET); // 等待发送完成
}

// 发送 16 位数据
// 参数:
//   pUSARTx: 串口外设指针
//   data: 要发送的 16 位数据
void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t data)
{
    uint16_t tmp_h = data >> 8; // 提取高 8 位
    uint16_t tmp_l = data & 0xFF; // 提取低 8 位
    Usart_SendByte(pUSARTx, tmp_h); // 发送高 8 位
    Usart_SendByte(pUSARTx, tmp_l); // 发送低 8 位
}

// 发送字节数组
// 参数:
//   pUSARTx: 串口外设指针
//   arr: 要发送的字节数组
//   num: 数组长度
void Usart_SendArray(USART_TypeDef *pUSARTx, uint8_t *arr, uint16_t num)
{
    while (num--)
    {
        Usart_SendByte(pUSARTx, *arr++); // 发送当前字节并移动指针
    }
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET); // 等待传输完成
}

// 发送字符串
// 参数:
//   pUSARTx: 串口外设指针
//   str: 要发送的字符串（以空字符 '\0' 结尾）
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
    while (*str != '\0')
    {
        Usart_SendByte(pUSARTx, *str++); // 发送当前字符并移动指针
    }
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET); // 等待传输完成
}

// 重定向 printf 到串口
// 参数:
//   ch: 要发送的字符
//   f: 文件指针（未使用）
// 返回值: 发送的字符
int fputc(int ch, FILE *f)
{
    USART_SendData(DEBUG_USARTx, (uint8_t)ch); // 发送字符到串口
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET); // 等待发送完成
    return ch;
}

// 重定向 scanf 和 getchar 到串口
// 参数:
//   f: 文件指针（未使用）
// 返回值: 接收到的字符
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET); // 等待接收数据
    return (int)USART_ReceiveData(DEBUG_USARTx); // 返回接收到的字符
}

// 串口中断服务函数
// 处理接收中断并回显数据
void DEBUG_USART_IRQHandler(void)
{
    if (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) != RESET)
    {
        uint16_t tmp = USART_ReceiveData(DEBUG_USARTx); // 读取接收到的数据
        USART_SendData(DEBUG_USARTx, tmp); // 回显数据
        while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET); // 等待发送完成
    }
}

#endif