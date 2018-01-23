#include "Font_Test.h"
#include "siffont.h"
#include "xbffont.h"

extern uint8_t Font_Test1[];
extern uint8_t Font_Test2[];
extern uint8_t Demo_Str1[];
extern uint8_t Demo_Str2[];

void Font_Demo()
{
	GUI_UC_SetEncodeUTF8();
	
	Create_XBF20("1:/Font/XBF20.xbf");
	
	GUI_SetFont(&XBF20_Font);
	GUI_DispString(Font_Test1);
	GUI_DispString(Demo_Str1);
	GUI_DispString(Demo_Str2);
	
	
//Create_SIF20("1:/Font/SIF20.sif");	
//	GUI_SetFont(&SIF20_Font);
//	GUI_DispString(Font_Test2);
//	GUI_DispString(Demo_Str1);
//	GUI_DispString(Demo_Str2);
}



