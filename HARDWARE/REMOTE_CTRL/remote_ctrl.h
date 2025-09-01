#ifndef __REMOTE_H
#define __REMOTE_H
#include "stdio.h"
#include "stm32f10x.h"

extern u8 front_flag;       //前进命令
extern u8 back_flag;        //后退命令
extern u8 turn_left_flag;   //左转命令
extern u8 turn_right_flag;  //右转命令
extern u8 stop_flag;        //停止命令
extern u8 voice_flag;       //音视频播放命令
void MSG_Analyze_BT(void);
void UI(void);


#endif