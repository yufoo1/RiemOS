#ifndef _USER_FD_H_
#define _USER_FD_H_ 1
#include "../../include/type.h"
#include "fs.h"
#include "../../include/memory.h"
#include "file.h"

#define debug 0

#define MAXFD 32
#define FILEBASE 0x60000000
#define FDTABLE (FILEBASE-PDMAP)

#define INDEX2FD(i)	(FDTABLE+(i)*PAGE_SIZE)
#define INDEX2DATA(i)	(FILEBASE+(i)*PDMAP)

#define USED(x) (void)(x)

// pre-declare for forward references
struct Fd;
struct Stat;
struct Dev;

// Device struct:
// It is used to read and write data from corresponding device.
// We can use the five functions to handle data.
// There are three devices in this OS: file, console and pipe.
struct Dev {
	int dev_id;
	char *dev_name;
	int (*dev_read)(int, u_longlong , u_longlong , u_longlong);
	int (*dev_write)(int, u_longlong , u_longlong , u_longlong);
	int (*dev_close)(struct Fd *);
	int (*dev_stat)(struct Fd *, struct Stat *);
	int (*dev_seek)(struct Fd *, u_int);
};

// file descriptor
struct Fd {
	u_int fd_dev_id;
	u_int fd_offset;
	u_int fd_omode;
};

// State
struct Stat {
	char st_name[MAXNAMELEN];
	u_int st_size;
	u_int st_isdir;
	struct Dev *st_dev;
};

// file descriptor + file
struct Filefd {
	struct Fd f_fd;
	u_int f_fileid;
	struct File f_file;
};

int fd_alloc(struct Fd **fd);
int fd_lookup(int fdnum, struct Fd **fd);
u_int fd2data(struct Fd *);
int fd2num(struct Fd *);
int dev_lookup(int dev_id, struct Dev **dev);
int	seek(int fd, u_int offset);
int num2fd(int fd);
static struct Dev devcons;
static struct Dev devfile;
static struct Dev devpipe;

static struct Dev *devtab[] = {
        &devfile,
        &devcons,
        &devpipe,
        0
};

#endif
