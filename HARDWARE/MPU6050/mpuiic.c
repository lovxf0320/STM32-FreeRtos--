#include "mpuiic.h"
#include "delay.h"

//陀螺仪IIC延时，计数值，不是毫秒
#define MPU_DELAY 1

//MPU_IIC 延时函数
void MPU_IIC_Delay(void){
    u8 i;
    for(i=0;i<MPU_DELAY;i++);
}

//初始化IIC
void MPU_IIC_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    //先使能外设IO PORTB时钟

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);			//PB10,PB11 输出高电平
    // 为什么设置高电平：
    // I2C 协议要求在空闲状态下，SCL 和 SDA 都保持高电平（通过外部上拉电阻或推挽输出）。
    // 初始化时将 SCL 和 SDA 设置为高电平，确保进入 I2C 空闲状态，准备后续的起始信号（SDA 从高到低，SCL 保持高）。
}


void MPU_IIC_Start(void){
    MPU_SDA_OUT();      // 将 SDA 引脚（PB11）配置为输出模式。
    MPU_IIC_SDA = 1;    // 设置 SDA 引脚为高电平。
    MPU_IIC_SCL = 1;    // 设置 SCL 引脚（PB10）为高电平。
    MPU_IIC_Delay();    // 延时，确保电平稳定，满足 I2C 时序要求。
    MPU_IIC_SDA = 0;    // 在 SCL 为高电平时，将 SDA 从高电平拉低。
    // 生成 I2C 起始信号
    MPU_IIC_Delay();    // 延时，确保起始信号被从设备识别
    MPU_IIC_SCL = 0;    // 将 SCL 拉低，钳住 I2C 总线，准备发送或接收数据
}


//产生IIC停止信号
void MPU_IIC_Stop(void){
    MPU_SDA_OUT();      // 将 SDA 引脚（PB11）配置为输出模式
    MPU_IIC_SCL = 0;    // 设置 SCL 引脚（PB10）为低电平
    MPU_IIC_SDA = 0;    // 设置 SDA 引脚为低电平
    MPU_IIC_Delay();    // 延时，确保电平稳定
    MPU_IIC_SCL = 1;    // 将 SCL 拉高
    // 生成 I2C 停止信号
    MPU_IIC_SDA = 1;    // 在 SCL 为高电平时，将 SDA 从低电平拉高，
    MPU_IIC_Delay();    // 延时，确保停止信号被从设备识别
}


//产生ACK应答
void MPU_IIC_ACK(void){
    MPU_IIC_SCL=0;
    MPU_SDA_OUT();
    MPU_IIC_SDA=0;
    MPU_IIC_Delay();
    MPU_IIC_SCL=1;
    MPU_IIC_Delay();
    MPU_IIC_SCL=0;
}


//产生NACK应答
void MPU_IIC_NACK(void){
    MPU_IIC_SCL=0;
	MPU_SDA_OUT();
	MPU_IIC_SDA=1;
	MPU_IIC_Delay();
	MPU_IIC_SCL=1;
	MPU_IIC_Delay();
	MPU_IIC_SCL=0;
}


//主机IIC主机发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void MPU_IIC_Send_Byte(u8 txd){
    u8 t;
    
    // ===== 准备阶段 =====
    MPU_SDA_OUT();                      // 【STM32操作】设置SDA为输出模式，准备发送数据
    MPU_IIC_SCL=0;                      // 【STM32操作】拉低SCL，确保开始前时钟为低电平
    
    // ===== 数据发送阶段：逐位发送8个数据位 =====
    for(t=0;t<8;t++){
        // 数据准备（SCL=0期间）
        MPU_IIC_SDA=(txd&0x80)>>7;      // 【STM32操作】提取最高位(bit7)并输出到SDA线
                                        // 【MPU6050状态】在SCL=0期间可以采样SDA准备数据
        txd<<=1;                        // 【STM32操作】数据左移1位，准备下一位数据

        // 数据有效期（SCL=1期间）  
        MPU_IIC_SCL=1;                  // 【STM32操作】拉高SCL，告诉从机"数据有效，请采样"
                                        // 【MPU6050操作】在SCL上升沿采样SDA线上的数据位
		MPU_IIC_Delay();                // 【STM32操作】延时，保持SCL=1期间数据稳定
        
        // 数据过渡期（SCL=0期间）
        MPU_IIC_SCL=0;                  // 【STM32操作】拉低SCL，告诉从机"准备接收下一位"
                                        // 【MPU6050状态】SCL=0期间，主机可以改变SDA数据
		MPU_IIC_Delay();                // 【STM32操作】延时，为下一位数据准备时间
    }
    
    // 注意：函数结束后通常需要调用MPU_IIC_Wait_Ack()来处理第9个时钟周期的应答信号
    // 此时SDA控制权会暂时交给从机(MPU6050)发送ACK/NACK
}


//主机读1个字节
//假如：
//读取MPU6050的6字节加速度数据
//u8 data[6];
//前5个字节发送ACK，告诉从机继续发送
//data[0] = MPU_IIC_Read_Byte(1);  // ACK - 继续
//data[1] = MPU_IIC_Read_Byte(1);  // ACK - 继续  
//data[2] = MPU_IIC_Read_Byte(1);  // ACK - 继续
//data[3] = MPU_IIC_Read_Byte(1);  // ACK - 继续
//data[4] = MPU_IIC_Read_Byte(1);  // ACK - 继续
//data[5] = MPU_IIC_Read_Byte(0);  // NACK - 结束传输
//这个代码都是stm32读取消息，读取一个字节，并且发送一个NACK
u8 MPU_IIC_Read_Byte(unsigned char ack){
    unsigned char i,receive==0;
    MPU_SDA_IN();           //SDA设置为输入
    for(i=0;i<8;i++){
        MPU_IIC_SCL=0;      //拉低时钟 - 让从机准备数据
        MPU_IIC_Delay();
		MPU_IIC_SCL=1;      //拉高时钟 - 告诉从机"现在可以采样了"
        receive<<=1;        //左移准备接收新位
        if(MPU_READ_SDA)    //在SCL=1期间读取数据(关键时刻!)
            receive++; 
        MPU_IIC_Delay();
    }
    if(!ack){
        MPU_IIC_NAck();     //意思是主设备（STM32）发送NACK给从设备
    }else{
        MPU_IIC_Ack();
    }
    return receive;
}



// 等待 I2C 从设备的应答信号（ACK）
// 返回值：1 表示接收应答失败（从设备未拉低 SDA 或超时）
//         0 表示接收应答成功（从设备拉低 SDA）
// 主机发完8位数据后，释放SDA控制权
u8 MPU_IIC_Wait_Ack(void){
    u8 ucErrTime=0;             //定义超时计数器，记录等待时间

    // 主机不再控制SDA，让从机控制
    MPU_SDA_IN();               //将 SDA 引脚（PB11）配置为输入模式，以便读取从设备信号
    // 主机提供时钟，让从机在这个时钟下发送应答
    MPU_IIC_SDA=1;              //在MPU_SDA_IN()中是让出总线
    MPU_IIC_Delay();
    MPU_IIC_SCL=1;
    MPU_IIC_Delay();

    // 读取从机的应答：0=收到，1=未收到
    while(MPU_READ_SDA){
        ucErrTime++;
        if(ucErrTime>250){
            MPU_IIC_Stop();
            return 1;
        }
    }
    MPU_IIC_SCL=0;
    return 0;
}

