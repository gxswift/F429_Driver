#ifndef _TOUCH_H
#define _TOUCH_H

#include "stm32f4xx_hal.h"
#include "GUI.h"


#define Response_OK 1  //IIC响应
#define Response_ERR 0

#define Touch_DelayVaule 4  //通讯延时时间

// IO口相关宏
#define IIC_Touch_SCL_CLK     RCC_AHB1Periph_GPIOC
#define IIC_Touch_SCL_PORT    GPIOC                 
#define IIC_Touch_SCL_PIN     GPIO_Pin_13   
         
#define IIC_Touch_SDA_CLK     RCC_AHB1Periph_GPIOB 
#define IIC_Touch_SDA_PORT    GPIOB                   
#define IIC_Touch_SDA_PIN     GPIO_Pin_2          


	
/*---------------------- GT9XX芯片相关定义 ------------------------*/
					
#define TOUCH_MAX   1	//最大触摸点数 5

typedef struct 
{
	uint8_t  flag;	//触摸标志位，为1时表示有触摸操作
	uint8_t  num;		//触摸点数
	uint16_t x[TOUCH_MAX];	//x坐标
	uint16_t y[TOUCH_MAX];	//y坐标
}TouchStructure;


#define GT9XX_CFG_ADDR 	0x8047		// 固件配置信息寄存器和配置起始地址

#define GT9XX_IIC_RADDR 0xBB	//IIC初始化地址
#define GT9XX_IIC_WADDR 0xBA

#define GT9XX_READ_ADDR 0x814E	//触摸信息寄存器
#define GT9XX_ID_ADDR 0x8140		//触摸面板ID寄存器

void Touch_Init(void);
uint8_t Touch43_Init(void);
uint8_t Touch_Scan(void);
void	GUI_TouchScan(void);


#endif
