#include "pwm.h"

void TIM1_PWM_Init(){
    //第一步：声明结构体变量
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //定时器时基结构体
    GPIO_InitTypeDef GPIO_InitTypeStructure;        //GPIO结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;          //定时器输出比较结构体


    //第二步：使能时钟
    //使能TIM1和GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOA,ENABLE);


    //第三步：配置GPIO引脚
    //配置GPIO为复用推挽输出模式
    GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_AF_PP;       //复用推挽输出模式
    GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;//PA10和PA11引脚(TIM1_CH3和TIM1_CH4)
    GPIO_InitTypeStructure.GPIO_Speed=GPIO_Speed_50HZ;      //GPIO速度设置为50MHz
    GPIO_Init(GPIOA,&GPIO_InitTypeStructure);


    //第四步：配置定时器时基
    //配置定时器基本参数
    TIM_TimeBaseStructure.TIM_Period=7200;                      //自动重装载值(ARR=7200)，决定PWM周期
    TIM_TimeBaseStructure.TIM_Prescaler=0;                      //预分频值，0表示不分频
    TIM_TimeBaseStructure.TIM_ClockDivision=0;                  //时钟分割，TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;   //向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;              //重复计数器值
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);              //初始化定时器时基


    //第五步：配置预装载
    //使能ARR预装载功能
    TIM_ARRPreloadConfig(TIM1,ENABLE);                          //允许在定时器工作时向ARR缓冲器写入新值


    //第六步：配置PWM功能
    //配置PWM输出通道参数
    TIM_OCInitStructure.TIM1_OCMode=TIM_OCMode_PWM1;                //PWM模式1
    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;     //输出使能
    TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Disable;  //互补输出禁用
    TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;      //空闲状态输出低电平
    TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;    //互补端空闲状态输出低电平
    TIM_OCInitStructure.TIM_Pulse = 0;                              //初始脉冲值设为0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性为高
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;     //互补端输出极性为高


    //第七步：初始化PWM通道
    //初始化TIM1的通道3（PA10）
    TIM_OC3Init(TIM1,&TIM_OCInitStructure);           //初始化通道3
    TIM_OC3PreloadConfog(TIM1,TIM_OCPreload_Enable);  //使能通道3预装载

    //初始化TIM1的通道4（PA11）
    TIM_OC4Init(TIM1,&TIM_OCInitStructure);           //初始化通道4
    TIM_OC4PreloadConfog(TIM1,TIM_OCPreload_Enable);  //使能通道4预装载


    //第八步：设置初始参数
    //设置定时器重装载值
    TIM_SetAutoreload(TIM1,7200);         //设置ARR值为7200,和上面的ARR冲突了

    //设置初始化占空比
    TIM_SetCompare3(TIM1, 1000);          // 设置通道3比较值，设置CCR
    TIM_SetCompare4(TIM1, 1000);          // 设置通道4比较值，设置CCR


    //第九步：使能中断和定时器
    //使能更新中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);         // 使能TIM1更新中断

    //使能定时器
    TIM_Cmd(TIM1, ENABLE);                             // 使能TIM1

    //使能PWM输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);                  // 使能PWM输出
    // 使能更新中断（TIM_ITConfig），当计数器达到 ARR 值时触发中断（代码中未实现中断服务函数）。
    // 启动 TIM1（TIM_Cmd），开始计数。
    // 使能 PWM 输出（TIM_CtrlPWMOutputs），将 PWM 信号输出到引脚
}


