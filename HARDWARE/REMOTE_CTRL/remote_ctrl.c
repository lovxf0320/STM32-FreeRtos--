#include "remote_ctrl.h"
#include "usart2.h"
#include "motor.h"

u8 front_flag;       //前进命令
u8 back_flag;        //后退命令
u8 turn_left_flag;   //左转命令
u8 turn_right_flag;  //右转命令
u8 stop_flag;        //停止命令
u8 voice_flag;       //音视频播放命令
int turnmentleft=0;
int turnmentright=0;


//数据分析
void MSG_Analyze_BT(void){
    //停车
    if(WF_Data1==0x01){
        WF_Data1=0;
        stop_flag=1;
        //USART3_Count=0;
    }
    //前进
    if(WF_Data1==0x02){
        WF_Data1=0;
        front_flag=1;
        //USART3_Count=0;
    }
    //后退
    if(WF_Data1==0x03){
        WF_Data1=0;
        back_flag=1;
        //USART3_Count=0;
    }
    //左转
    if(WF_Data1==0x04){
        WF_Data1=0;
        turn_left_flag=1;
        //USART3_Count=0;
    }
    //右转
    if(WF_Data1==0x05){
        WF_Data1=0;
        turn_right_flag=1;
        //USART3_Count=0;
    }
}


//UI交互
void UI(void){

}