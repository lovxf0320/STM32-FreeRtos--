#include "mpu6050.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

//初始化MPU6050
//返回值：0表示成功，其他值表示错误代码
u8 MPU_Init(void){
    u8 res;
    GPIO_InitTypeDef GPIO_InitStructure;

    //步骤 1: 使能 AFIO 和 GPIOA 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
    //步骤 2: 配置 PA15 引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    //步骤 3: 禁用 JTAG 功能，使PA15可用作普通IO
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
    //步骤 4: 设置 AD0 引脚为低电平
    MPU_AD0_CTRL=0;     //设置AD0为低电平
    // AD0=0时，MPU6050的I2C地址为0x68
    // AD0=1时，MPU6050的I2C地址为0x69

    //步骤 5: 初始化 I2C 总线
    MPU_IIC_Init();         //初始化IIC总线

    //步骤 6: 复位 MPU6050
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0x80); //复位MPU6050(设置电源管理寄存器1的bit7为1)
    //#define MPU_PWR_MGMT1_REG		0X6B	//电源管理寄存器1

    //步骤 7: 延时等待复位完成
    delay_ms(100);

    //步骤 8: 唤醒 MPU6050
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050（清零电源管理寄存器1）

    //步骤 9: 配置传感器量程和采样率
    MPU_Set_Gyro_Fsr(3);    //设置陀螺仪满量程为±2000dps
    MPU_Set_Accel_Fsr(0);   //设置加速度计满量程为±2g
    MPU_Set_Rate(50);       //设置采样率为50Hz

    //步骤 10: 关闭辅助功能
    MPU_Write_Byte(MPU_INT_EN_REG, 0X00);   //向中断使能寄存器（0x38）写入 0x00，关闭所有中断。
    MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00);//向用户控制寄存器（0x6A）写入 0x00，关闭 I2C 主模式（MPU6050 通常作为从设备）
    MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);  //向 FIFO 使能寄存器（0x23）写入 0x00，关闭 FIFO 缓冲。
    MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80);//向中断配置寄存器（0x37）写入 0x80，设置 INT 引脚为低电平有效。

    //步骤 11: 验证设备 ID
    res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if(res==MPU_ADDR){
        MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);     //设置时钟源为PLL X轴（CLKSEL=001）
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);    //启用加速度计和陀螺仪
        MPU_Set_Rate(50);                           //再次设置采样率为50Hz
    }else{
        return 1;
    }

    return 0;
}


// 设置MPU6050陀螺仪满量程范围
// fsr: 0=±250dps, 1=±500dps, 2=±1000dps, 3=±2000dps
// 返回值: 0表示成功，其他值表示失败
u8 MPU_Set_Gyro_Fsr(u8 fsr){
    return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3); //设置陀螺仪满量程（左移3位写入寄存器）
}


// 设置MPU6050加速度计满量程范围
// fsr: 0=±2g, 1=±4g, 2=±8g, 3=±16g
// 返回值: 0表示成功，其他值表示失败
u8 MPU_Set_Accel_Fsr(u8 fsr){
    return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3); //设置加速度计满量程（左移3位写入寄存器）
}


// 获取陀螺仪数据（原始值）
// gx, gy, gz: 指向X、Y、Z轴陀螺仪数据的指针
// 返回值: 0表示成功，其他值表示错误代码
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz){
    u8 buf[6],res;
    res=MPU_Read_Len(MPU_ADDR,6,buf);
    if(res==0){
        *gx=((u16)bug[0]<<8)|buf[1];
        *gx=((u16)bug[3]<<8)|buf[2];
        *gx=((u16)bug[5]<<8)|buf[4];
    }

    return res;
}


// 获取加速度计数据（原始值）
// ax, ay, az: 指向X、Y、Z轴加速度数据的指针
// 返回值: 0表示成功，其他值表示错误代码
u8 MPU_Get_Acceleroometer(short *ax,short *ay,short *az){
    u8 buf[6],res;
    res=MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
    if(res==0){
        *ax=((u16)bug[0]<<8)|buf[1];
        *ax=((u16)bug[3]<<8)|buf[2];
        *az=((u16)bug[5]<<8)|buf[4];
    }

    return res;
}


// 获取温度值
// 返回值: 温度值（放大100倍，单位为摄氏度）
short MPU_Get_Temperature(void){
    u8 buf[2];
    short raw;
    float temp;
    MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);  //读取温度寄存器（高低8位）

    raw = ((u16)buf[0] << 8) | buf[1];  // 合并高低位为16位数据
    temp = 36.53 + ((double)raw) / 340; // 根据公式转换为摄氏度
    return temp * 100;                  // 放大100倍返回
}


// I2C连续写入
// addr: 器件地址
// reg: 寄存器地址
// len: 写入数据长度
// buf: 数据缓冲区
// 返回值: 0表示成功，其他值表示错误代码
u8 MPU_Write_Len(u8 addr, u8 reg, u8 len, u8 *buf)
{
    u8 i; 
    MPU_IIC_Start();                    // 启动I2C
    MPU_IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
    if (MPU_IIC_Wait_Ack()){            // 等待应答
        MPU_IIC_Stop(); 
        return 1;                       // 应答失败
    }
    MPU_IIC_Send_Byte(reg); // 发送寄存器地址
    MPU_IIC_Wait_Ack();     // 等待应答

    for (i = 0; i < len; i++){
        MPU_IIC_Send_Byte(buf[i]);  // 发送数据
        if (MPU_IIC_Wait_Ack()){    // 等待应答
            MPU_IIC_Stop(); 
            return 1;       // 应答失败
        }        
    }    
    MPU_IIC_Stop();         // 停止I2C
    return 0;               // 写入成功
}


// I2C连续读取
// addr: 器件地址
// reg: 寄存器地址
// len: 读取数据长度
// buf: 数据存储缓冲区
// 返回值: 0表示成功，其他值表示错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 *buf){
    MPU_IIC_Start();                //启动I2C
    MPU_IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if (MPU_IIC_Wait_Ack()){        // 等待应答
        MPU_IIC_Stop(); 
        return 1;                   // 应答失败
    }
    MPU_IIC_Send_Byte(reg);         // 发送寄存器地址
    MPU_IIC_Wait_Ack();             // 等待应答

    MPU_IIC_Start();
    MPU_IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
    MPU_IIC_Wait_Ack();             //等待应答
    while(len){
        if(len==1){
            *buf=MPU_IIC_Read_Byte(0);
        }else{
            *buf=MPU_IIC_Read_Byte(1);
        }
        len--;
        buf++;
    }
    MPU_IIC_Stop();
    return 0;
}


// 设置MPU6050采样率（假定陀螺仪输出频率为1KHz）
// rate: 采样率，范围4~1000Hz
// 返回值: 0表示成功，其他值表示失败
u8 MPU_Set_Rate(u16 rate){
    u8 data;
    if(rate>1000){
        rate=1000;      //设置最大采样率
    }
    if (rate < 4){
        rate = 4;       //限制最小采样率
    }

    data=1000/data-1;   //计算采样率分频值：SMPLRT_DIV = 1000 / rate - 1
    data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);  //设置采样率分频器
    return MPU_Set_LPF(rate/2);
}


// 设置MPU6050数字低通滤波器
// lpf: 数字低通滤波频率(Hz)
// 返回值: 0表示成功，其他值表示失败
u8 MPU_Set_LPF(u16 lpf){
    u8 data=0;

    //根据输入频率选择滤波器设置
    if(lpf>=188){
        data=1;
    }else if(lpf>=98){
        data=2;
    }else if(lpf>=42){
        data=3;
    }else if(lpf>=20){
        data=4;
    }else if(lpf>=10){
        data=5;
    }else{
        data=6;
    }

    return MPU_Write_Byte(MPU_CFG_REG,data);    //设置数字低通滤波器
}


// I2C写入单个字节
// reg:寄存器地址
// data:要写入的数据
// 返回值:0表示成功，其他值表示错误代码
u8 MPU_Write_Byte(u8 reg,u8 data){
    MPU_IIC_Start();
    MPU_IIC_Send_Byte(MPU_ADDR<<1|0);
    if(MPU_IIC_Wait_Ack()){
        MPU_IIC_Stop();
        return 1;
    }

    MPU_IIC_Send_Byte(reg);
    MPU_IIC_Wait_Ack();
    MPU_IIC_Send_Byte(data);

    if(MPU_IIC_Wait_Ack()){
        MPU_IIC_Stop();
        return 1;
    }
    MPU_IIC_Stop();
    return 0;
}


// 主机I2C读取单个字节
// reg: 寄存器地址
// 返回值: 读取到的数据
U8 MPU_Read_Byte(u8 reg){
    u8 res;
    
    // ===== 第一阶段：写入寄存器地址 =====
    MPU_IIC_Start();                    // 【STM32操作】主机发送START条件，开始I2C通信
    MPU_IIC_Send_Byte(MPU_ADDR<<1|0);   // 【STM32操作】主机发送器件地址+写位(0)，告诉MPU6050准备接收寄存器地址
    MPU_IIC_Wait_Ack();                 // 【STM32操作】主机等待从机ACK确认 | 【MPU6050操作】从机发送ACK表示收到地址
    MPU_IIC_Send_Byte(reg);             // 【STM32操作】主机发送要读取的寄存器地址
    MPU_IIC_Wait_Ack();                 // 【STM32操作】主机等待从机ACK确认 | 【MPU6050操作】从机发送ACK表示收到寄存器地址

    // ===== 第二阶段：从指定寄存器读取数据 =====
    MPU_IIC_Start();                    // 【STM32操作】主机发送重复START条件，不释放总线，开始读取操作
    MPU_IIC_Send_Byte((MPU_ADDR << 1) | 1); // 【STM32操作】主机发送器件地址+读位(1)，告诉MPU6050准备发送数据
    MPU_IIC_Wait_Ack();                 // 【STM32操作】主机等待从机ACK确认 | 【MPU6050操作】从机发送ACK表示准备好发送数据
    res=MPU_IIC_Read_Byte(0);           // 【STM32操作】主机读取1个字节并发送NACK | 【MPU6050操作】从机发送寄存器数据，收到NACK停止发送
    MPU_IIC_Stop();                     // 【STM32操作】主机发送STOP条件，结束I2C通信，释放总线
    
    return res;                         // 【STM32操作】主机返回读取到的数据
}