#include "siffont.h"
#include "ff.h"

//SIF字体 先转到内存中

//字体定义
GUI_FONT SIF20_Font;
//字体文件
FIL SIF20FontFile;

//字库文件缓冲区
uint8_t SIF20FontBuff[4*1024*1024]__attribute__((at(0xD0465000+0x400000)));
//uint8_t SIF16FontBuff[3*1024*1024];
//uint8_t SIF24FontBuff[3*1024*1024];
//uint8_t SIF36FontBuff[3*1024*1024];

//创建SIF24字体，供EMWIN使用
void Create_SIF20(uint8_t *fxpath) 
{
	uint16_t bread;
	f_open(&SIF20FontFile,(const TCHAR*)fxpath,FA_READ);	
	f_read(&SIF20FontFile,SIF20FontBuff,SIF20FontFile.obj.objsize,(UINT *)&bread);
	
	GUI_SIF_CreateFont(SIF20FontBuff,&SIF20_Font,GUI_SIF_TYPE_PROP_AA2);
	f_close(&SIF20FontFile);
}



