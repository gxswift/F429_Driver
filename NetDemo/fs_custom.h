#ifndef _FS_CUSTOM_H
#define _FS_CUSTOM_H
#include "lwip/apps/httpd_opts.h"
#include "lwip/def.h"
#include "lwip/apps/fs.h"

int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);
int fs_read_custom(struct fs_file *file, char *buffer, int count);


#endif
