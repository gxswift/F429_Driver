#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "stm32f4xx_hal.h"
#define 	LCD_CLK  		25		//定义LCD驱动时钟，这里为了方便计算数值应在10-70之间，单位为M
#define 	LCD_NUM_LAYERS  1		//定义显示的层数，429可驱动两层显示
/*
#define	ColorMode_0   LCD_RGB565   		//定义层0的颜色格式
//#define	ColorMode_0   LCD_ARGB1555   
//#define	ColorMode_0   LCD_RGB888  
//#define	ColorMode_0   LCD_ARGB8888   

#if  LCD_NUM_LAYERS == 2						
//	#define	ColorMode_1   LCD_RGB565   	//定义层1的颜色格式
//	#define	ColorMode_1   LCD_ARGB1555   
// #define	ColorMode_1   LCD_RGB888   
	#define	ColorMode_1   LCD_ARGB8888   
#endif

#define	Mode_H	0		//LCD水平显示
#define	Mode_V	1		//LCD竖屏显示
#define  Fill_Zero  0	//填充0
#define  Fill_Space 1	//填充空格

#define	LCD_ARGB1555    3   //定义颜色格式
#define	LCD_RGB565      2   
#define	LCD_RGB888      1   
#define	LCD_ARGB8888    0  


#if ( ColorMode_0 == LCD_RGB565 || ColorMode_0 == LCD_ARGB1555 )
	#define BytesPerPixel_0		2		//16位色模式每个像素占2字节
#elif ColorMode_0 == LCD_RGB888
	#define BytesPerPixel_0		3		//24位色模式每个像素占3字节
#else
	#define BytesPerPixel_0		4		//32位色模式每个像素占4字节
#endif	

#if LCD_NUM_LAYERS == 2

	#if ( ColorMode_1 == LCD_RGB565 || ColorMode_1 == LCD_ARGB1555 )
		#define BytesPerPixel_1		2	//16位色模式每个像素占2字节
	#elif ColorMode_1 == LCD_RGB888	
		#define BytesPerPixel_1		3	//24位色模式每个像素占3字节
	#else	
		#define BytesPerPixel_1		4	//32位色模式每个像素占4字节
	#endif	

	#define LCD_MemoryAdd_OFFSET   LCD_Pixels * BytesPerPixel_0 	 //第二层的显存的偏移地址 
	#define LCD_Buffer	LCD_MemoryAdd + LCD_MemoryAdd_OFFSET + LCD_Pixels * BytesPerPixel_1	//开双层时，缓冲层的起始地址
#else
	#define LCD_Buffer	LCD_MemoryAdd + LCD_Pixels * BytesPerPixel_0	//只开单层时，缓冲层的起始地址
#endif*/
/*-------------------------- 常用颜色 ------------------------------*/

#define 	LCD_WHITE       0xffFFFFFF
#define 	LCD_BLACK       0xff000000
                           
#define 	LCD_BLUE        0xff0000FF
#define 	LCD_GREEN       0xff00FF00
#define 	LCD_RED         0xffFF0000
#define 	LCD_CYAN        0xff00FFFF
#define 	LCD_MAGENTA     0xffFF00FF
#define 	LCD_YELLOW      0xffFFFF00
#define 	LCD_GREY        0xff2C2C2C
                           
#define 	LIGHT_BLUE      0xff8080FF
#define 	LIGHT_GREEN     0xff80FF80
#define 	LIGHT_RED       0xffFF8080
#define 	LIGHT_CYAN      0xff80FFFF
#define 	LIGHT_MAGENTA   0xffFF80FF
#define 	LIGHT_YELLOW    0xffFFFF80
#define 	LIGHT_GREY      0xffA3A3A3
                           
#define 	DARK_BLUE       0xff000080
#define 	DARK_GREEN      0xff008000
#define 	DARK_RED        0xff800000
#define 	DARK_CYAN       0xff008080
#define 	DARK_MAGENTA    0xff800080
#define 	DARK_YELLOW     0xff808000
#define 	DARK_GREY       0xff404040

#define LCD_Width     800				//LCD的像素长度
#define LCD_Height    480				//LCD的像素宽度
#define LCD_Pixels    800*480 		//分辨率
#define LCD_MemoryAdd   0xD0000000 	//显存的起始地址  


void  LCD_Clear(void); //清屏

void  LCD_SetLayer(uint8_t Layerx); 					//设置层
void  LCD_Set_Color(uint32_t Color); 				   //设置画笔颜色
void  LCD_SetBackColor(uint32_t Color);  			//设置背景颜色


void  LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color);   				//画点
uint32_t 	LCD_ReadPoint(uint16_t x,uint16_t y);								//读点
void  LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		//画线
void  LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);	//画矩形
void  LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r);					//画圆
void  LCD_DrawEllipse(int x, int y, int r1, int r2);		//画椭圆

void Display_Init(void);
void Display_Test(void);

#endif
