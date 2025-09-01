#include "encoder.h"

//这里采用TIM2和TIM3的CH1和CH2通道进行编码器的接口输入
//定时器             CH1             CH2
//TIM2              PA0             PA1
//TIM3              PA6             PA7

//这里定时器2、3挂载在APB1上
//编码器时钟初始化
void Encoder_Counter_RCC(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
}


//编码器引脚初始化
void Encoder_Count_GPIO(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    //TIM2,PA0,PA1
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    //TIM3,PA6,PA7
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
}

//编码器功能初始化
void Encoder_Count_Configuration(void){
    //1.声明配置结构体
    TIM_ICInitTypeDef TIM_ICInitStruct;                 //用于配置定时器的输入捕获（Input Capture）参数。
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;     //用于配置定时器的时基（Time Base）参数，如周期和分频。

    //TIM2,PA0,PA1
    //2.初始化输入捕获结构体
    TIM_ICStructInit(&TIM_ICInitStruct);

    //3.配置输入捕获通道（TIM2: PA0, PA1；TIM3: PA6, PA7）
    TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICFilter=0xF;                  //滤波
    TIM_ICInit(TIM2,&TIM_ICInitStruct);

    TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
    TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICFilter=0xF;                  //滤波
    TIM_ICInit(TIM2,&TIM_ICInitStruct);

    //4.配置编码器模式
    //配置定时器进入编码器接口模式
    TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
    
    //5.配置时基
    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;  //时钟不分频，直接使用输入时钟。
    TIM_TimeBaseInitStruct.TIM_Period=65535;                //设置计数器周期为65536（16位计数器，最大值65535）。计数器达到此值后会溢出并重置。
    TIM_TimeBaseInitStruct.TIM_Prescaler=0;                 //预分频器为0，即不分频，计数器直接以系统时钟频率计数。
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;         //重复计数器设为0（仅用于高级定时器，此处无实际作用）。
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);         //应用时基配置到TIM2。

    //6.使能定时器
    TIM_Cmd(TIM2,ENABLE);

    //7.TIM3配置
    //TIM3,PA6,PA7
    TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICFilter=0xF;                  //滤波
    TIM_ICInit(TIM3,&TIM_ICInitStruct);

    TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
    TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICFilter=0xF;                  //滤波
    TIM_ICInit(TIM3,&TIM_ICInitStruct);

    //配置定时器进入编码器接口模式
    TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
    
    //时基配置
    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;  //时钟不分频，直接使用输入时钟。
    TIM_TimeBaseInitStruct.TIM_Period=65535;                //设置计数器周期为65536（16位计数器，最大值65535）。计数器达到此值后会溢出并重置。
    TIM_TimeBaseInitStruct.TIM_Prescaler=0;                 //预分频器为0，即不分频，计数器直接以系统时钟频率计数。
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;         //重复计数器设为0（仅用于高级定时器，此处无实际作用）。
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);         //应用时基配置到TIM2。

    //使能定时器
    TIM_Cmd(TIM3,ENABLE);
}


//**********************编码器初始化*********************
void Encoder_Counter_Init(void){
    Encoder_Counter_RCC();
    Encoder_Count_GPIO();
    Encoder_Count_Configuration();
}


//******************编码器数据读取********************************
//读取计数器的值
//读取旋转编码器的位置或速度信息
int Encoder_Value(TIM_TypeDef* TIMx){
    int Channel_val=0;
    //读取当前计数：编码器模式下，CNT随方向自增/自减
    //从指定定时器（TIMx，如TIM2或TIM3）的计数器寄存器（CNT）获取当前编码器计数。
    Channel_val=TIMx->CNT;

    if(Channel_val>>15){
        Channal_val=(Channal_val&0x7FFF)-32767;
    }

    return Channel_val
}


//****************编码器清零*************************************
void Encoder_Count_Clear(TIM_TypeDef* TIMx)
{
    // 将计数器清零（设定新的参考点）
    TIMx ->CNT = 0;
}