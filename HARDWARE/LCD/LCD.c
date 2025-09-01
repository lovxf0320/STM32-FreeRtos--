#include "LCD.h"
#include "delay.h"
#include "font.h"
// 使用的是 SPI 的 Mode0
// CPOL=0: 空闲时时钟为低电平
// CPHA=0: 在第1个时钟边沿（上升沿）采样数据

//液晶IO初始化配置
void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能相关 GPIO 端口的时钟
    // RCC_APB2PeriphClockCmd 启用指定外设的时钟，这里启用了 LCD 控制引脚所在的 GPIO 端口时钟
    RCC_APB2PeriphClockCmd(LCD_SCL_RCC | LCD_SDA_RCC | LCD_RES_RCC | \
                           LCD_DC_RCC | LCD_CS_RCC | LCD_BLK_RCC, ENABLE);

    // 配置 GPIO 初始化结构体
    GPIO_InitStructure.GPIO_Pin = LCD_SCL_PIN;          // SCL 引脚（SPI 时钟线）
                                                        // SCL (Serial Clock) 是 SPI 接口的时钟信号线，用于同步数据传输
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 设置为推挽输出模式
                                                        // 推挽输出提供高低电平的强驱动能力，适合控制 LCD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 设置 GPIO 输出速度为 50MHz
                                                        // 高速输出适合 SPI 通信的快速切换
    GPIO_Init(LCD_SCL_GPIO_PORT, &GPIO_InitStructure);  // 初始化 SCL 引脚所在的 GPIO 端口


    GPIO_InitStructure.GPIO_Pin = LCD_SDA_PIN;          // SDA 引脚（SPI 数据线）
                                                        // SDA (Serial Data) 是 SPI 接口的数据线，用于传输数据到 LCD
    GPIO_Init(LCD_SDA_GPIO_PORT, &GPIO_InitStructure);  // 初始化 SDA 引脚所在的 GPIO 端口
                                                        // 复用之前的模式和速度（推挽输出，50MHz）


    GPIO_InitStructure.GPIO_Pin = LCD_RES_PIN;          // RES 引脚（复位线）
                                                        // RES (Reset) 用于复位 LCD 模块，通常低电平有效
    GPIO_Init(LCD_RES_GPIO_PORT, &GPIO_InitStructure);  // 初始化 RES 引脚所在的 GPIO 端口


    GPIO_InitStructure.GPIO_Pin = LCD_DC_PIN;           // DC 引脚（数据/命令选择线）
                                                        // DC (Data/Command) 用于区分发送的是数据还是命令
                                                        // 通常高电平表示数据，低电平表示命令
    GPIO_Init(LCD_DC_GPIO_PORT, &GPIO_InitStructure);   // 初始化 DC 引脚所在的 GPIO 端口


    GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;           // CS 引脚（片选线）
                                                        // CS (Chip Select) 用于选择 LCD 模块，通常低电平选中
    GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStructure);   // 初始化 CS 引脚所在的 GPIO 端口


    GPIO_InitStructure.GPIO_Pin = LCD_BLK_PIN;          // BLK 引脚（背光控制线）
                                                        // BLK (Backlight) 用于控制 LCD 背光亮灭
                                                        // 通常高电平点亮背光，低电平关闭背光
    GPIO_Init(LCD_BLK_GPIO_PORT, &GPIO_InitStructure);  // 初始化 BLK 引脚所在的 GPIO 端口
}


//向SPI总线传输一个8位数据
//#define LCD_SDA  PBout(9)
//#define LCD_SCL  PBout(8)
void SPI_WriteData(u8 Data){
    usigned char i=0;

    for(i=8;i>0;i--){
        if(Data&0x80){
            LCD_SDA=1
        }else{
            LCD_SDA=0;
        }

        LCD_SCL=0;
        LCD_SCL=1;
        Data<<=1;
    }
}


//向lcd屏幕写入一个8位的指令
void LCD_WriteIndex(u8 Index){
    //SPI 写命令时序开始
    LCD_CS=0;           //拉低片选选中LCD
    LCD_DC=0;           //拉低DC表示发送命令字节
    SPI_WriteData(Index);
    LCD_CS=1;
}


//向lcd屏幕写入一个8位的数据
void LCD_WriteData(u8 Data){
    LCD_CS=0;
    LCD_DC=0;
    SPI_WriteData(Data);
    LCD_CS=1;
}


//向lcd屏幕写入一个16位的数据
void LCD_WriteData_16Bit(u16 Data){
    LCD_CS=0;
    LCD_DC=0;
    SPI_WriteData(Data>>8);
    SPI_WriteData(Data);
    LCD_CS=1;
}


void LCD_WriteReg(u8 Index, u8 Data)
{
  LCD_WriteIndex(Index);
  LCD_WriteData(Data);
}


//复位
void LCD_Reset(void){
    LCD_RES=0;
    delay_ms(100);
    LCD_RES=1;
    delay_ms(50);
}


//LCD初始化，具体参考数据手册
void LCD_Init(void){
    LCD_GPIO_Init();
    LCD_Reset(); //Reset before LCD Init.

    //LCD Init For 1.44Inch LCD Panel with ST7735R.
    LCD_WriteIndex(0x11);//Sleep exit
    delay_ms (120);

    //ST7735R Frame Rate
    LCD_WriteIndex(0xB1);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);

    LCD_WriteIndex(0xB2);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);

    LCD_WriteIndex(0xB3);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);

    LCD_WriteIndex(0xB4); //Column inversion
    LCD_WriteData(0x07);

    //ST7735R Power Sequence
    LCD_WriteIndex(0xC0);
    LCD_WriteData(0xA2);
    LCD_WriteData(0x02);
    LCD_WriteData(0x84);
    LCD_WriteIndex(0xC1);
    LCD_WriteData(0xC5);

    LCD_WriteIndex(0xC2);
    LCD_WriteData(0x0A);
    LCD_WriteData(0x00);

    LCD_WriteIndex(0xC3);
    LCD_WriteData(0x8A);
    LCD_WriteData(0x2A);
    LCD_WriteIndex(0xC4);
    LCD_WriteData(0x8A);
    LCD_WriteData(0xEE);

    LCD_WriteIndex(0xC5); //VCOM
    LCD_WriteData(0x0E);

    LCD_WriteIndex(0x36); //MX, MY, RGB mode
    LCD_WriteData(0xC8);

    //ST7735R Gamma Sequence
    LCD_WriteIndex(0xe0);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x1a);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x18);
    LCD_WriteData(0x2f);
    LCD_WriteData(0x28);
    LCD_WriteData(0x20);
    LCD_WriteData(0x22);
    LCD_WriteData(0x1f);
    LCD_WriteData(0x1b);
    LCD_WriteData(0x23);
    LCD_WriteData(0x37);
    LCD_WriteData(0x00);
    LCD_WriteData(0x07);
    LCD_WriteData(0x02);
    LCD_WriteData(0x10);

    LCD_WriteIndex(0xe1);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x1b);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x17);
    LCD_WriteData(0x33);
    LCD_WriteData(0x2c);
    LCD_WriteData(0x29);
    LCD_WriteData(0x2e);
    LCD_WriteData(0x30);
    LCD_WriteData(0x30);
    LCD_WriteData(0x39);
    LCD_WriteData(0x3f);
    LCD_WriteData(0x00);
    LCD_WriteData(0x07);
    LCD_WriteData(0x03);
    LCD_WriteData(0x10);

    LCD_WriteIndex(0x2a);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x7f);

    LCD_WriteIndex(0x2b);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x9f);

    LCD_WriteIndex(0xF0); //Enable test command
    LCD_WriteData(0x01);
    LCD_WriteIndex(0xF6); //Disable ram power save mode
    LCD_WriteData(0x00);

    LCD_WriteIndex(0x3A); //65k mode
    LCD_WriteData(0x05);

    LCD_WriteIndex(0x29);//Display on
}


/*************************************************
    函数名：LCD_Set_Region
    功能：设置lcd显示区域，在此区域写点数据自动换行
    入口参数：xy起点和终点
    返回值：无
*************************************************/
void LCD_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end){
    LCD_WriteIndex(0x2a);
    LCD_WriteData(0x00);
    LCD_WriteData(x_start+2);
    LCD_WriteData(0x00);
    LCD_WriteData(x_end+2);

    LCD_WriteIndex(0x2b);
    LCD_WriteData(0x00);
    LCD_WriteData(y_start+3);
    LCD_WriteData(0x00);
    LCD_WriteData(y_end+3);

    LCD_WriteIndex(0x2c);
}


/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void LCD_SetXY(u16 x,u16 y){
    LCD_SetRegion(x,y,x,y);
}


/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 Data){
    LCD_SetRegion(x, y, x + 1, y + 1);
    LCD_WriteData_16Bit(Data);
}


/*****************************************
 函数功能：读TFT某一点的颜色
 出口参数：color  点颜色值
******************************************/
unsigned int LCD_ReadPoint(u16 x,u16 y){
    unsigned int Data;
    LCD_SetXY(x,y);     //设置lcd显示起始点

    LCD_WriteData(Data);
    return Data;
}


/*************************************************
函数名：LCD_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void LCD_Clear(u16 Color){
    unsigned int i,m;
    LCD_SetRegion(0,0,X_MAX_PIX-1,Y_MAX_PIXEL-1);
    LCD_WriteIndex(0x2C);
    for(i=0;i<X_MAX_PIX;i++){
        for(m=0;m<Y_MAX_PIXEL;m++){
            LCD_WriteData_16Bit(Color);
        }
    }

}


//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c){
    u16 r,g,b,rgb;
    b=(c>>0)&0x1f;
    g=(c>>0)&0x3f;
    r=(c>>11)&0x1f;
    rgb=(b<<11)+(g<<5)+(r<<0);
    return rgb;
}


//画圆
void LCD_Circle(u16 X, u16 Y, u16 R, u16 fc){
    //Bresenham算法
    unsigned short  a, b;
    int c;
    a = 0;
    b = R;
    c = 3 - 2 * R;

    while (a < b){
        LCD_DrawPoint(X + a, Y + b, fc); //        7
        LCD_DrawPoint(X - a, Y + b, fc); //        6
        LCD_DrawPoint(X + a, Y - b, fc); //        2
        LCD_DrawPoint(X - a, Y - b, fc); //        3
        LCD_DrawPoint(X + b, Y + a, fc); //        8
        LCD_DrawPoint(X - b, Y + a, fc); //        5
        LCD_DrawPoint(X + b, Y - a, fc); //        1
        LCD_DrawPoint(X - b, Y - a, fc); //        4

        if(c < 0){
            c = c + 4 * a + 6;
        }else{
            c = c + 4 * (a - b) + 10;
            b -= 1;
        }
        a += 1;
    }
    if (a == b){
        LCD_DrawPoint(X + a, Y + b, fc);
        LCD_DrawPoint(X + a, Y + b, fc);
        LCD_DrawPoint(X + a, Y - b, fc);
        LCD_DrawPoint(X - a, Y - b, fc);
        LCD_DrawPoint(X + b, Y + a, fc);
        LCD_DrawPoint(X - b, Y + a, fc);
        LCD_DrawPoint(X + b, Y - a, fc);
        LCD_DrawPoint(X - b, Y - a, fc);
    }
}


//画线函数，使用Bresenham 画线算法
void LCD_DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color){
    int dx,             // difference in x's
        dy,             // difference in y's
        dx2,            // dx,dy * 2
        dy2,
        x_inc,          // amount in pixel space to move during drawing
        y_inc,          // amount in pixel space to move during drawing
        error,          // the discriminant i.e. error i.e. decision variable
        index;          // used for looping


    LCD_SetXY(x0, y0);
    dx = x1 - x0; //计算x距离
    dy = y1 - y0; //计算y距离

    if (dx >= 0){
        x_inc = 1;
    }else{
        x_inc = -1;
        dx    = -dx;
    }

    if (dy >= 0){
        y_inc = 1;
    }else{
        y_inc = -1;
        dy    = -dy;
    }

    dx2 = dx << 1;
    dy2 = dy << 1;

    if (dx > dy){//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
        //且线的点数等于x距离，以x轴递增画点
        // initialize error term
        error = dy2 - dx;

        // draw the line
        for (index = 0; index <= dx; index++){ //要画的点数不会超过x距离
            //画点
            LCD_DrawPoint(x0, y0, Color);

            // test if error has overflowed
            if (error >= 0){ //是否需要增加y坐标值
                error -= dx2;

                // move to next line
                y0 += y_inc; //增加y坐标值
            } // end if error overflowed

            // adjust the error term
            error += dy2;

            // move to the next pixel
            x0 += x_inc; //x坐标值每次画点后都递增1
        }   // end for
    }else{  //y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
        //以y轴为递增画点
        // initialize error term
        error = dx2 - dy;

        // draw the line
        for (index = 0; index <= dy; index++){
            // set the pixel
            LCD_DrawPoint(x0, y0, Color);

            // test if error overflowed
            if (error >= 0){
                error -= dy2;
                
                // move to next line
                x0 += x_inc;
            } // end if error overflowed

            // adjust the error term
            error += dx2;

            // move to the next pixel
            y0 += y_inc;
        } // end for
    } // end else |slope| > 1
}


//画矩形
void LCD_box(u16 x,u16 y,u16 w,u16 h,u16 bc){
  LCD_DrawLine(x, y, x + w, y, 0xEF7D);
  LCD_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);
  LCD_DrawLine(x, y + h, x + w, y + h, 0x2965);
  LCD_DrawLine(x, y, x, y + h, 0xEF7D);
  LCD_DrawLine(x + 1, y + 1, x + 1 + w - 2, y + 1 + h - 2, bc);
}


//画矩形
void LCD_box2(u16 x,u16 y,u16 w,u16 h,u8 mode){
    if (mode == 0){
        LCD_DrawLine(x, y, x + w, y, 0xEF7D);
        LCD_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);
        LCD_DrawLine(x, y + h, x + w, y + h, 0x2965);
        LCD_DrawLine(x, y, x, y + h, 0xEF7D);
    }
    if (mode == 1){
        LCD_DrawLine(x, y, x + w, y, 0x2965);
        LCD_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xEF7D);
        LCD_DrawLine(x, y + h, x + w, y + h, 0xEF7D);
        LCD_DrawLine(x, y, x, y + h, 0x2965);
    }
    if (mode == 2){
        LCD_DrawLine(x, y, x + w, y, 0xffff);
        LCD_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xffff);
        LCD_DrawLine(x, y + h, x + w, y + h, 0xffff);
        LCD_DrawLine(x, y, x, y + h, 0xffff);
    }
}


/**************************************************************************************
功能描述: 在屏幕显示一凸起的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonDown(u16 x1,u16 x2,u16 y1,u16 y2){
    LCD_DrawLine(x1,  y1,  x2, y1, GRAY2); //H
    LCD_DrawLine(x1 + 1, y1 + 1, x2, y1 + 1, GRAY1); //H
    LCD_DrawLine(x1,  y1,  x1, y2, GRAY2); //V
    LCD_DrawLine(x1 + 1, y1 + 1, x1 + 1, y2, GRAY1); //V
    LCD_DrawLine(x1,  y2,  x2, y2, WHITE); //H
    LCD_DrawLine(x2,  y1,  x2, y2, WHITE); //V
}


/**************************************************************************************
功能描述: 在屏幕显示一凹下的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2){
    LCD_DrawLine(x1,  y1,  x2, y1, WHITE); //H
    LCD_DrawLine(x1,  y1,  x1, y2, WHITE); //V

    LCD_DrawLine(x1 + 1, y2 - 1, x2, y2 - 1, GRAY1); //H
    LCD_DrawLine(x1,  y2,  x2, y2, GRAY2); //H
    LCD_DrawLine(x2 - 1, y1 + 1, x2 - 1, y2, GRAY1); //V
    LCD_DrawLine(x2, y1, x2, y2, GRAY2);  //V
}


/**
 * LCD字体和图像显示函数库
 * 包含16x16中英文字体、24x24中英文字体、32x32数字和图像显示功能
 */

/**
 * 显示16x16像素的中英文字符
 * 
 * @param x  字符显示的起始横坐标
 * @param y  字符显示的起始纵坐标
 * @param fc 字体前景色
 * @param bc 字体背景色
 * @param s  要显示的字符串指针
 * 
 * 说明：
 * - ASCII字符占用8x16像素
 * - 中文字符占用16x16像素
 * - 支持换行符(ASCII 13)自动换行
 */
 void LCD_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
 {
     unsigned char i, j;
     unsigned short k, x0;
     x0 = x;  // 保存起始横坐标，用于换行时重置
 
     while (*s) {
         // 处理ASCII字符 (0-127)
         if ((*s) < 128) {
             k = *s;
             
             // 处理换行符
             if (k == 13) {
                 x = x0;      // 横坐标回到行首
                 y += 16;     // 纵坐标下移一行
             } else {
                 // 字符索引调整：ASCII 32以下为控制字符，显示为空格
                 if (k > 32) {
                     k -= 32;
                 } else {
                     k = 0;
                 }
 
                 // 绘制8x16的ASCII字符
                 for (i = 0; i < 16; i++) {
                     for (j = 0; j < 8; j++) {
                         if (asc16[k * 16 + i] & (0x80 >> j)) {
                             LCD_DrawPoint(x + j, y + i, fc);  // 绘制前景色像素
                         } else {
                             if (fc != bc) {
                                 LCD_DrawPoint(x + j, y + i, bc);  // 绘制背景色像素
                             }
                         }
                     }
                 }
                 x += 8;  // 横坐标右移8像素
             }
             s++;  // 指向下一个字符
         }
         // 处理中文字符 (128-255)
         else {
             // 在字库中查找匹配的中文字符
             for (k = 0; k < hz16_num; k++) {
                 if ((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1))) {
                     // 绘制16x16的中文字符
                     for (i = 0; i < 16; i++) {
                         // 绘制左半部分 (8像素)
                         for (j = 0; j < 8; j++) {
                             if (hz16[k].Msk[i * 2] & (0x80 >> j)) {
                                 LCD_DrawPoint(x + j, y + i, fc);
                             } else {
                                 if (fc != bc) {
                                     LCD_DrawPoint(x + j, y + i, bc);
                                 }
                             }
                         }
                         
                         // 绘制右半部分 (8像素)
                         for (j = 0; j < 8; j++) {
                             if (hz16[k].Msk[i * 2 + 1] & (0x80 >> j)) {
                                 LCD_DrawPoint(x + j + 8, y + i, fc);
                             } else {
                                 if (fc != bc) {
                                     LCD_DrawPoint(x + j + 8, y + i, bc);
                                 }
                             }
                         }
                     }
                     break;  // 找到字符后退出循环
                 }
             }
             s += 2;   // 中文字符占用2个字节
             x += 16;  // 横坐标右移16像素
         }
     }
 }
 
 /**
  * 显示24x24像素的中英文字符
  * 
  * @param x  字符显示的起始横坐标
  * @param y  字符显示的起始纵坐标
  * @param fc 字体前景色
  * @param bc 字体背景色
  * @param s  要显示的字符串指针
  * 
  * 说明：
  * - ASCII字符仍占用8x16像素
  * - 中文字符占用24x24像素
  */
 void LCD_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
 {
     unsigned char i, j;
     unsigned short k;
 
     while (*s) {
         // 处理ASCII字符
         if (*s < 0x80) {
             k = *s;
             
             // 字符索引调整
             if (k > 32) {
                 k -= 32;
             } else {
                 k = 0;
             }
 
             // 绘制8x16的ASCII字符
             for (i = 0; i < 16; i++) {
                 for (j = 0; j < 8; j++) {
                     if (asc16[k * 16 + i] & (0x80 >> j)) {
                         LCD_DrawPoint(x + j, y + i, fc);
                     } else {
                         if (fc != bc) {
                             LCD_DrawPoint(x + j, y + i, bc);
                         }
                     }
                 }
             }
             s++;
             x += 8;
         }
         // 处理中文字符
         else {
             // 在24x24字库中查找匹配的中文字符
             for (k = 0; k < hz24_num; k++) {
                 if ((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1))) {
                     // 绘制24x24的中文字符
                     for (i = 0; i < 24; i++) {
                         // 绘制第1列 (8像素)
                         for (j = 0; j < 8; j++) {
                             if (hz24[k].Msk[i * 3] & (0x80 >> j)) {
                                 LCD_DrawPoint(x + j, y + i, fc);
                             } else {
                                 if (fc != bc) {
                                     LCD_DrawPoint(x + j, y + i, bc);
                                 }
                             }
                         }
                         
                         // 绘制第2列 (8像素)
                         for (j = 0; j < 8; j++) {
                             if (hz24[k].Msk[i * 3 + 1] & (0x80 >> j)) {
                                 LCD_DrawPoint(x + j + 8, y + i, fc);
                             } else {
                                 if (fc != bc) {
                                     LCD_DrawPoint(x + j + 8, y + i, bc);
                                 }
                             }
                         }
                         
                         // 绘制第3列 (8像素)
                         for (j = 0; j < 8; j++) {
                             if (hz24[k].Msk[i * 3 + 2] & (0x80 >> j)) {
                                 LCD_DrawPoint(x + j + 16, y + i, fc);
                             } else {
                                 if (fc != bc) {
                                     LCD_DrawPoint(x + j + 16, y + i, bc);
                                 }
                             }
                         }
                     }
                     break;  // 找到字符后退出循环
                 }
             }
             s += 2;   // 中文字符占用2个字节
             x += 24;  // 横坐标右移24像素
         }
     }
 }
 
 
 /**
  * 显示32x32像素的数码管风格数字
  * 
  * @param x   数字显示的起始横坐标
  * @param y   数字显示的起始纵坐标
  * @param fc  数字前景色
  * @param bc  数字背景色
  * @param num 要显示的数字 (0-9)
  * 
  * 说明：数字以数码管样式显示，尺寸为32x32像素
  */
 void LCD_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
 {
     unsigned char i, j, k, c;
 
     // 绘制32x32的数字
     for (i = 0; i < 32; i++) {          // 32行
         for (j = 0; j < 4; j++) {       // 每行4个字节 (32像素)
             c = *(sz32 + num * 32 * 4 + i * 4 + j);  // 获取当前字节数据
             
             // 处理当前字节的8个像素
             for (k = 0; k < 8; k++) {
                 if (c & (0x80 >> k)) {
                     LCD_DrawPoint(x + j * 8 + k, y + i, fc);  // 绘制前景色
                 } else {
                     if (fc != bc) {
                         LCD_DrawPoint(x + j * 8 + k, y + i, bc);  // 绘制背景色
                     }
                 }
             }
         }
     }
 }
 

 /**
  * 显示位图图像
  * 
  * @param x0 图像显示的起始横坐标
  * @param y0 图像显示的起始纵坐标
  * @param x1 图像宽度
  * @param y1 图像高度
  * @param p  图像数据数组指针
  * 
  * 说明：
  * - 取模方式：水平扫描，从左到右，低位在前
  * - 图像数据为16位色彩格式
  * - 每个像素占用2个字节
  */
 void LCD_Showimage(u16 x0, u16 y0, u16 x1, u16 y1, const unsigned char *p)
 {
     int i;
     unsigned char picH, picL;
 
     // 设置显示区域：从起点到终点
     LCD_SetRegion(x0, y0, x0 + x1 - 1, y0 + y1 - 1);
     
     // 逐像素绘制图像
     for (i = 0; i < x1 * y1; i++) {
         picL = *(p + i * 2);       // 获取像素数据低8位
         picH = *(p + i * 2 + 1);   // 获取像素数据高8位
         
         // 合并高低位并写入LCD
         LCD_WriteData_16Bit(picH << 8 | picL);
     }
 }