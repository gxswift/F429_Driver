#include "xbffont.h"
#include "ff.h"



//定义字体
GUI_FONT XBF20_Font;

GUI_XBF_DATA	XBF20_Data;

FIL XBF20FontFile;

//回调函数，用来获取字体数据
//参数：Off:		在XBF中偏移(位置)
//		NumBytes:	要读取的字节数
//		pVoid:	要读取的文件
//		pBuff:	读取到的数据的缓冲区
//返回值:0 成功，1 失败
static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) 
{
	int result;
	uint16_t bread; 
	FIL *hFile;

	hFile = (FIL*)pVoid; 
	
	//设置在文件中的偏移(位置)
	result = f_lseek(hFile,Off);
	if(result != FR_OK)	return 1; //返回错误

	//读取字体数据
		
	result = f_read(hFile,pBuffer,NumBytes,(UINT *)&bread); //读取数据
	
	if(result != FR_OK) return 1; //返回错误
	return 0; 
}

//创建XBF20字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF20(uint8_t *fxpath) 
{
	int result;
	result = f_open(&XBF20FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件
	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF20_Font,    //指向GUI_FONT结构
						&XBF20_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP_AA2_EXT,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF20FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}

uint8_t Create_Font(void)
{
	Create_XBF20("0:/SYS/FONT/XBF12.xbf");
}

