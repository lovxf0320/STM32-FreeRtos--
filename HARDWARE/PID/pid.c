#include "pid.h"

PID_TypeDef g_location_pid;  //位置环PID参数结构体，控制小车位置
PID_TypeDef g_speed_pid;     //速度环PID参数结构体，控制小车速度
PID_TypeDef g_current_pid;   //电流环PID参数结构体，控制电机电流


// PID初始化
// 功能：初始化所有PID控制器的参数
// 说明：设置PID控制器的初始状态，包括目标值、输出值、误差值、PID系数等
void pid_init(void){
    // 初始化位置环PID参数 - 控制小车的位置精度
    g_location_pid.SetPoint = 0;        // 目标位置值，初始设为0
    g_location_pid.ActualValue = 0.0;   // PID计算后的输出值，初始为0
    g_location_pid.SumError = 0.0;      // 误差累计值，用于积分控制，初始为0
    g_location_pid.Error = 0.0;         // 当前误差值，初始为0
    g_location_pid.LastError = 0.0;     // 上一次误差值，初始为0
    g_location_pid.PrevError = 0.0;     // 上上次误差值，初始为0
    g_location_pid.Proportion = L_KP;   // 设置位置环比例系数
    g_location_pid.Integral = L_KI;     // 设置位置环积分系数
    g_location_pid.Derivative = L_KD;   // 设置位置环微分系数

    // 初始化速度环PID参数 - 控制小车的运动速度
    g_speed_pid.SetPoint = 1000;        // 目标速度值，初始设为1000（具体单位根据编码器确定）
    g_speed_pid.ActualValue = 0.0;      // PID计算后的输出值，初始为0
    g_speed_pid.SumError = 0.0;         // 误差累计值，初始为0
    g_speed_pid.Error = 0.0;            // 当前误差值，初始为0
    g_speed_pid.LastError = 0.0;        // 上一次误差值，初始为0
    g_speed_pid.PrevError = 0.0;        // 上上次误差值，初始为0
    g_speed_pid.Proportion = S_KP;      // 设置速度环比例系数
    g_speed_pid.Integral = S_KI;        // 设置速度环积分系数
    g_speed_pid.Derivative = S_KD;      // 设置速度环微分系数

    // 初始化电流环PID参数 - 控制电机的电流大小
    g_current_pid.SetPoint = 0.0;       // 目标电流值，初始设为0
    g_current_pid.ActualValue = 0.0;    // PID计算后的输出值，初始为0
    g_current_pid.SumError = 0.0;       // 误差累计值，初始为0
    g_current_pid.Error = 0.0;          // 当前误差值，初始为0
    g_current_pid.LastError = 0.0;      // 上一次误差值，初始为0
    g_current_pid.PrevError = 0.0;      // 上上次误差值，初始为0
    g_current_pid.Proportion = C_KP;    // 设置电流环比例系数
    g_current_pid.Integral = C_KI;      // 设置电流环积分系数
    g_current_pid.Derivative = C_KD;    // 设置电流环微分系数
}

//***************增量式PID控制函数************************
// 功能：实现增量式PID控制算法
// 参数说明：
//   PID：PID结构体变量地址，包含PID参数和状态
//   Feedback_value：当前实际反馈值（如当前位置、速度、电流等）
//   lose_flag：误差忽略标志，0=不忽略误差，1=忽略小于lose_n的误差
//   lose_n：可忽略的误差值范围
// 返回值：PID计算后的输出增量值
// 算法原理：增量式PID只输出控制量的增量，适合执行器为步进电机等增量式设备
int32_t increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value,u8 lose_flag,float lose_n){  //增量式PID控制算法
    //根据lose_flag判断是否忽略小误差
    //不忽略误差，直接计算偏差
    if(lose_flag==0){
        PID->Error=(float)(PID->SetPoint-Feedback_value);       //计算偏差=目标值-实际值
    }

    //忽略误差
    if(lose_flag==0){
        //当误差绝对值小于lose_n时，将误差设为0。
        PID->Error=(float)(PID->SetPoint-Feedback_value);
        if((PID->Error<=lose_n)&&(PID->Error>=-lose_n)){
            PID->Error=0;       //误差在可忽略范围内，设为0
        }
    }

    // 增量式PID算法：输出 = Kp*(当前误差-上次误差) + Ki*当前误差 + Kd*(当前误差-2*上次误差+上上次误差)
    PID->ActualValue += (PID->Proportion * (PID->Error - PID->LastError))                          // 比例环节：Kp*(e(k)-e(k-1))
                        + (PID->Integral * PID->Error)                                             // 积分环节：Ki*e(k)
                        + (PID->Derivative * (PID->Error - 2 * PID->LastError + PID->PrevError));  // 微分环节：Kd*(e(k)-2*e(k-1)+e(k-2))

    //更新误差
    PID->PrevError=PID->LastError;              //上上次误差 = 上次误差
    PID->LastError=PID->Error;                  //上次误差 = 当前误差

    return PID->ActualValue;
}


//***************位置式PID控制函数************************
// 功能：实现位置式PID控制算法
// 参数说明：
//   PID：PID结构体变量地址，包含PID参数和状态
//   Feedback_value：当前实际反馈值
//   lose_flag：误差忽略标志，0=不忽略误差，1=忽略小于lose_n的误差
//   lose_n：可忽略的误差值范围
// 返回值：PID计算后的绝对输出值
// 算法原理：位置式PID输出控制量的绝对值，适合执行器为伺服电机等位置控制设备
int32_t location_pid_ctrl(PID_TypeDef *PID,float Feedback_value,u8 lose_flag,float lose_n){
    //根据lose_flag判断是否忽略小误差
    //不忽略误差，直接计算偏差
    if(lose_flag==0){
        PID->Error=(float)(PID->SetPoint-Feedback_value);
    }

    else if(lose_flag==1){
        PID->Error=(float)(PID->SetPoint-Feedback_value);
        if((PID->Error<=lose_n)&&(PID->Error>=-lose_n))
        {
            PID->Error = 0;      // 误差设为0
            PID->SumError = 0;   // 积分累计值也设为0，防止积分饱和
        } 
    }

    // 累加误差值，用于积分控制
    PID->SumError += PID->Error;
    
    // 位置式PID算法：输出 = Kp*当前误差 + Ki*误差累计 + Kd*(当前误差-上次误差)
    PID->ActualValue = (PID->Proportion * PID->Error)                       // 比例环节：Kp*e(k)
                       + (PID->Integral * PID->SumError)                    // 积分环节：Ki*∑e(k)
                       + (PID->Derivative * (PID->Error - PID->LastError)); // 微分环节：Kd*(e(k)-e(k-1))
    
    // 更新误差历史值
    PID->LastError = PID->Error;                  // 上次误差 = 当前误差
    
    return ((int32_t)(PID->ActualValue));         // 返回计算后的输出值（转换为整数）
}


//*************积分限幅函数*********************************************
// 功能：限制PID控制器的积分累计值，防止积分饱和
// 参数说明：
//   PID：PID结构体变量地址
//   max_limit：积分上限值
//   min_limit：积分下限值
// 作用：当积分值超出范围时，将其限制在指定范围内，避免积分饱和导致的控制性能下降
void Integral_limit(PID_TypeDef *PID,float max_limit,float min_limit){
    if(PID->SumError>max_limit){
        PID->SumError==max_limit;       //积分值超过上限
    }else if(PID->SumError<min_limit){
        PID->SumError==min_limit;       //积分值超过下限
    }
}
