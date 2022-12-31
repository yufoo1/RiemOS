#ifndef __FILE_H
#define __FILE_H

#include "../../include/type.h"
#include "../../user/include/fd.h"


#define NDEV 4
#define NFILE 64 //Number of fd that all process can open
typedef struct file {
    enum { FD_NONE, FD_PIPE, FD_ENTRY, FD_DEVICE } type;
    int ref;  // reference count
    char readable;
    char writable;
    struct pipe* pipe;  // FD_PIPE
    struct dirent* ep;
    u_int off;     // FD_ENTRY
    short major;  // FD_DEVICE
} file;

#define major(dev) ((dev) >> 16 & 0xFFFF)
#define minor(dev) ((dev)&0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// map major device number to device functions.
//struct devsw {
//    int (*read)(int isUser, u_longlong dst, u_longlong start, u_longlong len);
//    int (*write)(int isUser, u_longlong src, u_longlong start, u_longlong len);
//};

//extern struct Dev devtab[];

#define DEV_SD 0
#define DEV_CONSOLE 1

#define MAXPATHLEN 1024

int file_open(char *path, struct File **pfile);
int file_get_block(struct File *f, u_int blockno, void **pblk);
int file_set_size(struct File *f, u_int newsize);
int file_remove(char *path);
int file_dirty(struct File *f, u_int offset);
void file_flush(struct File *);

void fs_init(void);
void fs_sync(void);
extern u_int *bitmap;
int map_block(u_int);
int alloc_block(void);

/* these are defined by POSIX and also present in glibc's dirent.h */
#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14



#endif