#ifndef __TOUCH_H
#define __TOUCH_H

#include "stm32f4xx.h"
#include "touch_iic.h"  
#include "usart.h"

/*------------------------------------ 相关定义 -----------------------------------*/  		

#define TOUCH_MAX   5	//最大触摸点数

typedef struct 
{
	u8  flag;			//	触摸标志位，为1时表示有触摸操作
	u8  num;				//	触摸点数
	u16 x[TOUCH_MAX];	//	x坐标
	u16 y[TOUCH_MAX];	//	y坐标
}TouchStructure;

extern TouchStructure touchInfo;	// 触摸数据结构体声明	

/*------------------------------------ 寄存定义 -----------------------------------*/  		

#define GT9XX_IIC_RADDR 0xBB			// IIC初始化地址
#define GT9XX_IIC_WADDR 0xBA

#define GT9XX_CFG_ADDR 	0x8047		// 固件配置信息寄存器和配置起始地址
#define GT9XX_READ_ADDR 0x814E		// 触摸信息寄存器
#define GT9XX_ID_ADDR 	0x8140		// 触摸面板ID寄存器

/*------------------------------------ 函数声明 -----------------------------------*/  		

u8 	Touch_Init(void);		// 触摸屏初始化
void 	Touch_Scan(void);    // 触摸扫描
void  GT9XX_Reset(void);   // 执行复位操作
void 	GT9XX_SendCfg(void);	// 发送GT9147配置参数
void 	GT9XX_ReadCfg(void);	// 读取GT9147配置参数
	
#endif

