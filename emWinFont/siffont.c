#include "siffont.h"
#include "ff.h"
//字体定义
GUI_FONT SIF12_Font;
GUI_FONT SIF16_Font;
GUI_FONT SIF24_Font;
GUI_FONT SIF36_Font;

//字体文件
FIL SIF12FontFile;
FIL SIF16FontFile;
FIL SIF24FontFile;
FIL SIF36FontFile;

//字库文件缓冲区
uint8_t SIF12FontBuff[3*1024*1024]__attribute__((at(0XC0600000)));
uint8_t SIF16FontBuff[3*1024*1024];
uint8_t SIF24FontBuff[3*1024*1024];
uint8_t SIF36FontBuff[3*1024*1024];

//创建SIF12字体，供EMWIN使用
void Create_SIF12(uint8_t *fxpath) 
{
   //创建SIF格式字体
   GUI_SIF_CreateFont(SIF12FontBuff,&SIF12_Font,GUI_SIF_TYPE_PROP_AA2);
}
     
//创建SIF16字体，供EMWIN使用
void Create_SIF16(uint8_t *fxpath) 
{
    GUI_SIF_CreateFont(SIF16FontBuff,&SIF16_Font,GUI_SIF_TYPE_PROP_AA2);
}

//创建SIF24字体，供EMWIN使用
void Create_SIF24(uint8_t *fxpath) 
{
GUI_SIF_CreateFont(SIF24FontBuff,&SIF24_Font,GUI_SIF_TYPE_PROP_AA2);
}

//创建SIF36字体，供EMWIN使用
void Create_SIF36(uint8_t *fxpath) 
{
    //创建SIF格式字体
    GUI_SIF_CreateFont(SIF36FontBuff,&SIF36_Font,GUI_SIF_TYPE_PROP_AA2);
}


