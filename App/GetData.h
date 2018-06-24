#ifndef GETDATA_H
#define GETDATA_H
#include "GUI.h"
#include "fatfs.h"

//Get BMP/GIF/JPEG Data
int Static_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off); 
/*
xSize = GUI_BMP_GetXSizeEx(Static_GetData,&BMP_File);
ySize = GUI_BMP_GetYSizeEx(Static_GetData,&BMP_File);
GUI_BMP_DrawEx(Static_GetData,&BMP_File,x,y);
*/

//Get PNG Bitstream graph Data
int Stream_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);


void PicDisplaly_Test(void);
#endif

