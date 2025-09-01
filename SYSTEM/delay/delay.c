#include "delay.h"
#include "sys.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"       //FreeRTOS核心头文件
#include "task.h"           //FreeRTOS任务管理头文件
#endif

static u8 fac_us=0;         //微秒（us）延时倍数因子，用于计算延时所需的SysTick计数
static u8 fac_ms=0;         //毫秒（ms）延时倍数因子，在使用OS时，表示每个OS时钟节拍的毫秒数

//声明外部函数，用于FreeRTOS的SysTick中断处理
extern void xPortSysTickHandler(void);

//SysTick中断服务函数，当使用FreeRTOS时调用
void SysTick_Handler(void){
    //检查FreeRTOS调度器是否已启动
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED){
        //如果调度器已启动，调用FreeRTOS的SysTick处理函数
        xPortSysTickHandler();
    }
}

//初始化SysTick定时器，用于精确延时
//SysTick的时钟源默认为AHB时钟（HCLK），在FreeRTOS中通常设置为AHB时钟（不分频）
//SYSCLK：系统时钟频率
void delay_init(){
    u32 reload;

    // 1. 配置 SysTick 的时钟源为 HCLK（即 CPU 主频，不分频）
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    
    
    // 2. 计算 SysTick 计数频率（以 us 为单位）
    fac_us = SystemCoreClock / 1000000;     // 1us 内 SysTick 走多少个计数
    
    // 3. 计算 SysTick 重装载值 reload
    reload = SystemCoreClock / 1000000;     // 先得到 1us 的计数数值
    reload *= 1000000 / configTICK_RATE_HZ; // 再乘以 “一个 tick 的微秒数”，得到 1个OS节拍所需的计数值
    
    // 4. 计算 fac_ms：1个 tick 对应多少 ms
    fac_ms = 1000 / configTICK_RATE_HZ;     
    
    // 5. 打开 SysTick 中断（计数到 0 时触发中断）
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;    
    
    // 6. 装载 SysTick 的重装值（决定每次中断周期）
    SysTick->LOAD = reload;
    
    // 7. 启动 SysTick 计数
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   
    
}

//微秒级延时函数
//参数nus:需要延时的微秒数，范围为0~204522252（受限于fac_us和32位寄存器最大值）
void delay_us(u32 nus){
    u32 ticks;
    u32 told,tnow,tcnt=0;

    //获取SysTick的重装载值
    u32 reload=SysTick->LOAD;
    //计算需要的SysTick计数次数（nus*fac_us）
    ticks=nus*fac_us;
    //记录当前SysTick计数器的值
    told=SysTick->VAL;

    while(1){
        //获取当前SysTick计数器的值
        tnow=SysTick->VAL;
        //如果计数器值发生变化（即SysTick计数器递减）
        if (tnow!=told){
            //如果当前值小于之前的值，说明计数器未发生溢出
            if(tnow<told){
                tcnt+=told-tnow;
            }
            //如果当前值大于之前的值，说明计数器发生溢出
            else{
                tcnt+=reload-tnow+told;
            }
            //更新told为当前值
            told=tnow;
            //如果累计的计数次数达到目标值，退出循环
            if (tcnt>=ticks){
                break;
            }
        }
    }
}

//毫秒级延时函数
//参数 nms: 需要延时的毫秒数，范围为0~65535
void delay_ms(u32 nms){
    //检查FreeRTOS调度器是否已启动
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED){
        //如果延时时间大于等于OS时钟节拍的毫秒数
        if(nms>=fac_ms){
            //使用FreeRTOS的任务延时函数，单位为OS时钟节拍
            vTaskDelay(nms/fac_ms);
        }
        nms%=fac_ms;
    }

    //使用微秒延时函数处理剩余的毫秒数（转换为微秒）
    delay_us((u32)(nms*1000));
}

//简单毫秒级延时函数（不依赖OS）
//参数nms：需要延时的毫秒数
void delay_xms(u32 nms){
    u32 i;
    //通过循环调用1毫秒的微秒延时实现毫秒延时
    for(i=0;i<nms;i++){
        delay_us(1000);
    }
}