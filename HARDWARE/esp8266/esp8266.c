#include "esp8266.h"
#include "string.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"

char a[]="AT+CWMODE=1";                                     //设置 ESP8266 的工作模式。
char b[]="AT+RST";                                          //重启 ESP8266 模块。
char c[]="AT+CWJAP=\"1\",\"11111111\"";                     //连接到指定的 Wi-Fi 网络。
char d[]="AT+CIPMUX=1";                                     //设置 ESP8266 的连接模式（单连接或多连接）。
char e[]="AT+CIPSTART=0;\"TCP\",\"192.168.61.141",6552"";   //建立 TCP 或 UDP 连接到指定服务器.
char f[]="AT+CWLAP";                                        //扫描当前可用的 Wi-Fi 网络。

void esp8266_start_trans(void){
    //重启
    esp8266_send_cmd1((u8 *)b);
    delay_xms(1000);
    delay_xms(1000);
    delay_xms(1000);
    delay_xms(1000);

    //设置工作模式 
    //Station模式（AT+CWMODE=1）：
    //ESP8266作为客户端，连接到现有的Wi-Fi网络（如家用路由器）。
    //AP模式（AT+CWMODE=2）：
    //ESP8266作为接入点（热点），其他设备可连接到它。
    //Station+AP模式（AT+CWMODE=3）：
    //同时支持Station和AP功能，ESP8266可连接到Wi-Fi网络并作为热点。
    esp8266_send_cmd1((u8 *)a);
    delay_xms(1000);
    delay_xms(1000);

    //重启
    esp8266_send_cmd1((u8 *)b);
    delay_xms(1000);
    delay_xms(1000);
    delay_xms(1000);
    delay_xms(1000);

    //连接WIFI
    esp8266_send_cmd1((u8 *)c);
    delay_xms(1000);
	delay_xms(1000);
	delay_xms(1000);
	delay_xms(1000);
	delay_xms(1000);
	delay_xms(1000);
	delay_xms(1000);
	delay_xms(1000);

    esp8266_send_cmd1((u8 *)d);
    delay_xms(1000);
	delay_xms(1000);

    esp8266_send_cmd1((u8 *)e);
    delay_xms(1000);
	delay_xms(1000);
}


void esp8266_send_cmd1(u8 *cmd){
    u2_printf("%s\r\n",cmd);
}