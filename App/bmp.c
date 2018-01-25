#include "bmp.h"

#include "ff.h"
#include "string.h"

static BYTE BMP_Head[54] = {
0x42,0x4d,0x36,0xb8,0xb,0x0,0x0,0x0,0x0,0x0,
0x36,0x0,0x0,0x0,0x28,0x0,0x0,0x0,0x20,0x3,
0x0,0x0,0xe0,0x1,0x0,0x0,0x1,0x0,0x10,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0};


static uint16_t ExchangeBytes(uint16_t value)
{
    uint16_t    tmp_value;
    uint8_t     *index_1, *index_2;

    index_1 = (uint8_t *)&tmp_value;
    index_2 = (uint8_t *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}
#define MERROR 1
static FIL fil;
static uint16_t Data_Buff[2048];
void Create_BMP()
{
	UINT bw;
	FRESULT res;
	char buf[20] = "0:/Screenshot.bmp";
	uint32_t i;
	uint8_t temp;
	temp = f_open(&fil,buf, FA_WRITE|FA_CREATE_ALWAYS);
	if(FR_OK == temp)
	{
		res = f_write(&fil,BMP_Head,sizeof (BMP_Head),&bw);
		//if(res||bw<54)return;
		#if MERROR
		for (i=0;i<=800*480*2/4096;i++)//镜像翻转
		{
			memcpy(Data_Buff,(const uint16_t*)( 0XD0000000+i*4096),sizeof Data_Buff);//一次写4K
			res = f_write(&fil,(uint8_t*)Data_Buff,4096,&bw);
		}
		#else
		for (i=800*480;i>0;i--)
		{
			memcpy(Data_Buff,(const uint16_t*)( 0XD0000000+i*2),2);//一次写4K
			res = f_write(&fil,(uint8_t*)Data_Buff,2,&bw);
		}
		#endif
		f_close(&fil);
	}
}


