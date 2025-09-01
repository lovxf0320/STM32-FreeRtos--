#ifndef __PID_H
#define __PID_H
#include "stm32f10x.h"
//#include "pbdata.h"

//PID参数设置
//位置环PID参数 - 用于控制小车的位置精度
#define L_KP 2.5f       //位置环比例系数，控制响应速度，值越大响应越快但可能超调
#define L_KI 0.0f       //位置环积分系数，消除稳态误差，当前设为0表示不使用积分控制
#define L_KD 0.2f       //位置环微分系数，抑制超调，提高系统稳定性

//速度环PID参数 - 用于控制小车的运动速度
#define  S_KP      0.03f             // 速度环比例系数，控制速度响应
#define  S_KI      0.02f             // 速度环积分系数，消除速度稳态误差
#define  S_KD      0.01f             // 速度环微分系数，抑制速度波动

//电流环PID参数 - 用于控制电机电流，实现精确的力矩控制
#define  C_KP      8.00f             // 电流环比例系数，控制电流响应速度
#define  C_KI      4.00f             // 电流环积分系数，消除电流稳态误差
#define  C_KD      1.00f             // 电流环微分系数，抑制电流波动

#define SMAPLSE_PID_SPEED 50         //PID采样周期，单位毫秒(ms)，即每50ms计算一次PID

//PID控制结构体 - 存储PID控制器的所有参数和状态
typedef struct{
    __IO float  SetPoint;            // 目标设定值，用户期望达到的目标
    __IO float  ActualValue;         // PID计算后的实际输出值
    __IO float  SumError;            // 误差累计值，用于积分控制
    __IO float  Proportion;          // 比例系数Kp，控制响应速度
    __IO float  Integral;            // 积分系数Ki，消除稳态误差
    __IO float  Derivative;          // 微分系数Kd，抑制超调，提高稳定性
    __IO float  Error;               // 当前误差值 Error[1]
    __IO float  LastError;           // 上一次误差值 Error[-1]
    __IO float  PrevError;           // 上上次误差值 Error[-2]
}PID_TypeDef;


//声明三个PID控制器的全局变量
extern PID_TypeDef g_location_pid;  //位置环PID参数结构体，控制小车位置
extern PID_TypeDef g_speed_pid;     //速度环PID参数结构体，控制小车速度
extern PID_TypeDef g_current_pid;   //电流环PID参数结构体，控制电机电流

//函数声明
void pid_init(void);
int32_t increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value,u8 lose_flag,float lose_n);  //增量式PID控制算法
int32_t location_pid_ctrl(PID_TypeDef *PID,float Feedback_value,u8 lose_flag,float lose_n);   //位置式PID控制算法
void integral_limit(PID_TypeDef *PID , float max_limit, float min_limit );       // 积分限幅函数，防止积分饱和


#endif