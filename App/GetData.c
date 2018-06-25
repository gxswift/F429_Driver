#include "GetData.h"

//use fatfs
//Get BMP/GIF/JPEG Data

static char acBuffer[0x400];//稍微大点  不然bmp无法显示 jpeg进入hardfault
int Static_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {

FIL * phFile;
UINT NumBytesRead;
phFile = (FIL *)p;
//
// Check buffer size
//
if (NumBytes > sizeof(acBuffer)) {
NumBytes = sizeof(acBuffer);
}
//
// Set file pointer to the required position
//
//	if(Off == 1) Off = 0;
f_lseek(phFile,Off);
//SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
//
// Read data into buffer
//
f_read(phFile,acBuffer,NumBytes,&NumBytesRead);
//ReadFile(*phFile, acBuffer, NumBytes, &NumBytesRead, NULL);
//
// Set data pointer to the beginning of the buffer
//
*ppData = (U8*)acBuffer;
//
// Return number of available bytes
//
return NumBytesRead;
}

/*

xSize = GUI_BMP_GetXSizeEx(Static_GetData,&BMP_File);
ySize = GUI_BMP_GetYSizeEx(Static_GetData,&BMP_File);
GUI_BMP_DrawEx(Static_GetData,&BMP_File,x,y);

*/

//Get PNG Bitstream graph Data
int Stream_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {
FIL * phFile;
UINT NumBytesRead;
U8 * pData;
pData = (U8 *)*ppData;
phFile = (FIL *)p;
//
// Set file pointer to the required position
//
f_lseek(phFile,Off);
//SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
//
// Read data into buffer
//
f_read(phFile,pData,NumBytes,&NumBytesRead);
//ReadFile(*phFile, pData, NumBytes, &NumBytesRead, NULL);
//
// Return number of available bytes
//
return NumBytesRead;
}
//TEST
/*
SD:0  FLASH:1
*/
#include "FreeRTOS.h"
#include "timers.h"
#include ""
char path1[20] = "0:/image/1.bmp";
char path2[20] = "0:/image/1.jpg";
char path3[20] = "0:/image/1.png";
static FIL BMP,JPG;

uint8_t TestBuffer[480*272*2+1000] __attribute__((at(0XD0400000)));
void PicDisplaly_Test(void)
{
	uint8_t res;
	UINT Read;
	//先加载到内存
	res = f_open (&BMP,path1,FA_READ);
//xSize = GUI_BMP_GetXSizeEx(Static_GetData,&BMP_File);
//ySize = GUI_BMP_GetYSizeEx(Static_GetData,&BMP_File);
	if (res == FR_OK)
	{
		f_read(&BMP,TestBuffer,BMP.obj.objsize,&Read);
	GUI_BMP_Draw(TestBuffer,0,0);
	f_close(&BMP);
		printf("internal bmp\r\n");
	vTaskDelay(2000);
	}
	else
	{
		printf("bmp file open error:%d\r\n",res);
	}
//------------------------------------------------------------	
	res = f_open (&BMP,path1,FA_READ);
	if (res == FR_OK)
	{
	GUI_BMP_DrawEx(Static_GetData,&BMP,250,50);
	f_close(&BMP);
		printf("extern bmp\r\n");
	vTaskDelay(2000);
	}
	else
	{
		printf("bmp file open error:%d\r\n",res);
	}
//------------------------------------------------------------	
//先加载到内存
	res = f_open (&JPG,path2,FA_READ);
//xSize = GUI_BMP_GetXSizeEx(Static_GetData,&BMP_File);
//ySize = GUI_BMP_GetYSizeEx(Static_GetData,&BMP_File);
	if (res == FR_OK)
	{
		f_read(&JPG,TestBuffer,JPG.obj.objsize,&Read);
	GUI_JPEG_Draw(TestBuffer,JPG.obj.objsize,200,0);
	f_close(&JPG);
		printf("internal jpg\r\n");
	vTaskDelay(2000);
	}
	else
	{
		printf("jpg file open error:%d\r\n",res);
	}
//------------------------------------------------------------		
	res = f_open (&JPG,path2,FA_READ);
	if (res == FR_OK)
	{
		GUI_JPEG_DrawEx(Static_GetData,&JPG,300,150);
		f_close(&JPG);
		printf("jpg display extern\r\n");
		vTaskDelay(2000);
	}
	else 
	{
		printf("jpg file open error:%d\r\n",res);
	}

}


