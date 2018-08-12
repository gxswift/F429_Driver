#include "tftp_file.h"
#include "ff.h"
#include "lwip/apps/tftp_server.h"

#include "string.h"
static FIL tftp_file;
static void *file_open(const char* fname, const char* mode, u8_t write)
{
	char name[40]={0};
	BYTE Write_Mode;
/*	
//filename error sometimes
	char namecopy[40];

	char *p;
	uint8_t i;

		memcpy(namecopy,fname,30);
	p = namecopy;
	for (i = 0;i < 20;i++)
	{
		if(*p++ == '.')
		break;
	}
	for (i = 0;i < 5;i++)
	{
		p++;
		if(*p<'A' ||*p>'z')
		{
			*p = '\0';
			break;
		}
	}
	sprintf(name,"0:/TFTP/%s",namecopy);
	*/
//------------------------------------------------------
	sprintf(name,"0:/TFTP/%s",fname);
	if(write) Write_Mode = FA_WRITE|FA_CREATE_ALWAYS|FA_READ	;
	else Write_Mode = FA_READ;
	
	f_open (&tftp_file,name,Write_Mode);
	return &tftp_file;
}

static void file_close(void* handle)
{
	f_close((FIL *)handle);
}

static int file_read(void* handle, void* buf, int bytes)
{
	UINT br;
	FRESULT res;
	res = f_read((FIL *)handle,buf,bytes,&br);
	if(res || br == 0)//error
		return -1;
	else
		return 0;
}
//直接返回正确速度快，但不能判断是否正确写入
static int file_write(void* handle, struct pbuf* p)
{
	UINT bw;
	FRESULT res;
	res = f_write((FIL *)handle,p->payload,p->len,&bw);
	if(res||bw < p->len)//error
		return -1;
	else
		return 0;	
}
struct tftp_context Tftp_Struct;
void TFTP_Config()
{
	Tftp_Struct.open  = file_open;
	Tftp_Struct.close = file_close;
	Tftp_Struct.read  = file_read;
	Tftp_Struct.write = file_write;
}

void TFTP_Start()
{
	TFTP_Config();
	tftp_init(&Tftp_Struct);
}

