#ifndef __MPUIIC_H
#define __MPUIIC_H

//IO方向设置
// 将 PB11（SDA 引脚）配置为输入模式（带上拉/下拉）
#define MPU_SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
//GPIOB->CRH&=0XFFFF0FFF: 清除 CRH 寄存器中第 12-15 位（对应 PB11 的配置位）
//GPIOB->CRH|=8<<12: 将第 12-15 位设置为 0b1000（十进制为 8），将 PB11 配置为输入模式（带上拉/下拉）

// 将 PB11（SDA 引脚）配置为输出模式（推挽输出）
#define MPU_SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
//GPIOB->CRH&=0XFFFF0FFF: 清除 CRH 寄存器中第 12-15 位（对应 PB11 的配置位）
//GPIOB->CRH|=3<<12: 将第 12-15 位设置为 0b0011（十进制为 3），将 PB11 配置为推挽输出模式

// IO操作函数
// IO 操作函数
#define MPU_IIC_SCL    PBout(10)  // SCL: 设置或清除 GPIOB 引脚 10（PB10）的输出电平，用于 I2C 的时钟线
#define MPU_IIC_SDA    PBout(11)  // SDA: 设置或清除 GPIOB 引脚 11（PB11）的输出电平，用于 I2C 的数据线
#define MPU_READ_SDA   PBin(11)   // 输入 SDA: 读取 GPIOB 引脚 11（PB11）的输入电平，用于检测 I2C 数据线的状态


//IIC所有操作函数
void MPU_IIC_Delay(void);				//MPU IIC延时函数
void MPU_IIC_Init(void);                //初始化IIC的IO口
void MPU_IIC_Start(void);				//发送IIC开始信号
void MPU_IIC_Stop(void);	  			//发送IIC停止信号
void MPU_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 MPU_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void MPU_IIC_Ack(void);					//IIC发送ACK信号
void MPU_IIC_NAck(void);				//IIC不发送ACK信号

void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);

#endif