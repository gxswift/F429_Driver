#include "fs_custom.h"
#include "fatfs.h"

#define FS_EXTERN 0//0:SD Card 1:SPI Flash
FIL File_Net;
int fs_open_custom(struct fs_file *file, const char *name)
{
    FRESULT fres;
    char buffer[100];
	#if FS_EXTERN==0
	sprintf((char *)buffer, "0:/Web_sever/%s", name);
	#elif FS_EXTERN == 1
	sprintf((char *)buffer, "1:/Web_sever/%s", name);
	#endif
    fres = f_open(&File_Net, buffer, FA_OPEN_EXISTING | FA_READ);

    if(fres != FR_OK)
    {
        return 0;
    }
    file->data = NULL;
    file->len = f_size(&File_Net);
    file->index = 0;
    file->pextension = NULL;

    return 1;
}

 

void fs_close_custom(struct fs_file *file)
{
    f_close(&File_Net);
}

 

int fs_read_custom(struct fs_file *file, char *buffer, int count)
{
    uint16_t DLen = 0;

    if(f_eof(&File_Net))
    {
        return(FS_READ_EOF);
    }
    if(file->index < file->len)
    {
        f_read(&File_Net, buffer, count, (void *)&DLen);
        return(DLen);
    }
    else
    {
        DLen = FS_READ_EOF;
    }
    return(DLen);
}
