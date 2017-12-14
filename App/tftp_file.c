#include "tftp_file.h"
#include "ff.h"
#include "lwip/apps/tftp_server.h"

static FIL tftp_file;
static FIL *file_open(const char* fname, const char* mode, u8_t write)
{
	char name[20];
	BYTE Write_Mode;
	if(write) Write_Mode = FA_WRITE|FA_CREATE_ALWAYS;
	else Write_Mode = FA_READ;
	sprintf(name,"0:/%s",fname);
	f_open (&tftp_file,name,Write_Mode);
	return &tftp_file;
}

static void file_close(void* handle)
{
	f_close(handle);
}

static int file_read(void* handle, void* buf, int bytes)
{
	UINT br;
	FRESULT res;
	res = f_read(handle,buf,bytes,&br);
	if(res || br<bytes)
		return 1;
	else
		return 0;
}

static int file_write(void* handle, struct pbuf* p)
{
	UINT bw;
		FRESULT res;
	res = f_write(handle,p->payload,p->len,&bw);
	if(res||bw<p->len)
		return 1;
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

