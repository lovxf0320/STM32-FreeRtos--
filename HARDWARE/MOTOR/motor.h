#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"

// ============================ 基本标志位 ============================
// 传感器数据读取是否正常（1: 正常，0: 异常）
extern u8 mpu6050_data_flag;

// ============================ 姿态/IMU数据 ============================
// 欧拉角（单位：deg）
extern float pitch, roll, yaw;
// 陀螺仪原始数据（单位：LSB）
extern short gyrox, gyroy, gyroz;
// MPU6050 温度原始值（单位：LSB）
extern short mpu6050_temp;

// ============================ 编码器/里程计 ============================
// 编码器瞬时读数（原始tick）
extern int bianma1;
extern int bianma2;
// 编码器累计脉冲数（总tick）
extern int bianma1_num;
extern int bianma2_num;
// 编码器溢出/中断计数（用于长时间累计）
extern int bianma_cnt1;
extern int bianma_cnt2;

// ============================ 速度估计/滤波 ============================
// 速度采样次数计数器
extern int speed_num1;
extern int speed_num2;
// 滤波后的速度均值（当前/上次），以及参与均值的样本个数
extern int speed_avr1;
extern int speed_last_avr1;
extern int speed_avr1_num;
// 速度滑动窗口缓存（用于均值滤波）
extern int speed_bga1[10];
// 速度采样计数（用于窗口索引或周期计数）
extern int speed_cnt1;
// 另一侧电机同上
extern int speed_avr2;
extern int speed_last_avr2;
extern int speed_avr2_num;
extern int speed_bga2[10];
extern int speed_cnt2;

// ============================ 控制量/执行输出 ============================
// 三个控制环输出（直立/速度/转向）
extern int sudu_out;   // 速度环输出
extern int zhili_out;  // 直立环输出
extern int zhuan_out;  // 转向环输出
// PWM 输出占空（或幅值），对应左右电机
extern int PWM_OUT1;    //左电机PWM值
extern int PWM_OUT2;    //右电机PWM值

// ============================ 运动状态/命令 ============================
// 前后运动意图量/档位（上层给定）
extern int Movement;
// 左右转向意图量/档位（上层给定）
extern int turnment;

// ============================ 显示/调试字符串 ============================
extern char pitch_str[20];
extern char roll_str[20];
extern char yaw_str[20]; // 欧拉角字符串显示缓存

// ============================ 机械零点标定 ============================
// 小车机械零点（姿态基准）
extern float Car_zero;
// 小车机械零点偏移值（安装误差补偿）
extern float Car_zero_offset;

// ============================ PID 参数 ============================
// 直立环（姿态稳定）
extern float zhili_Kp; // 比例：大则低频大幅振荡倾向增强（示例：-500）
extern float zhili_Kd; // 微分：大则高频小幅振荡倾向增强（示例：1.8）
// 速度环（速度/位姿积分）
extern float sudu_Kp;  // 建议后续做归一化（如除以200）
extern float sudu_Ki;  // 速度积分增益
// 转向环（航向/转角）
extern float zhuan_Kp;
extern float zhuan_Kd;

// ============================ 中间临时量 ============================
// PID 计算中的临时变量/调试监视
extern int PID_temp;

// ============================ 函数声明 ============================
void Encoder_Count_read(void);
void MPU6050_Data_read(void);
void Moto_Speed_Read(u8 n);
int my_abs(int n);
void Moto_Ctrl(int pwm,u8 n);
void Speed1_Read(void);
void Speed2_Read(void);
void Smart_Car_Task(void);
void Moto_Task(void);


#endif