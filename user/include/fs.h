// See COPYRIGHT for copyright information.

#ifndef _FS_H_
#define _FS_H_ 1

#ifndef _INC_TYPES_H_
#include "../../include/type.h"
#include "../../include/memConfig.h"
#endif
// File nodes (both in-memory and on-disk)

// Bytes per file system block - same as page size
#define BY2BLK		(PAGE_SIZE)
#define BIT2BLK		(BY2BLK * 8)

// Maximum size of a filename (a single path component), including null
#define MAXNAMELEN	128

// Maximum size of a complete pathname, including null
#define MAXPATHLEN	1024

// Number of (direct) block pointers in a File descriptor
#define NDIRECT		10
#define NINDIRECT	(BY2BLK/4)

#define MAXFILESIZE	(NINDIRECT*BY2BLK)

#define BY2FILE     256

struct File {
	u_char f_name[MAXNAMELEN];	// filename
	u_int f_size;			// file size in bytes
	u_int f_type;			// file type
	u_int f_direct[NDIRECT];
	u_int f_indirect;

	struct File *f_dir;		// the pointer to the dir where this file is in, valid only in memory.
	u_char f_pad[BY2FILE - MAXNAMELEN - 4 - 4 - NDIRECT * 4 - 4 - 4];
};

#define FILE2BLK	(PAGE_SIZE/sizeof(struct File))

// File types
#define FTYPE_REG		0	// Regular file
#define FTYPE_DIR		1	// Directory


// File system super-block (both in-memory and on-disk)

#define FS_MAGIC	0x68286097	// Everyone's favorite OS class

struct Super {
	u_int s_magic;		// Magic number: FS_MAGIC
	u_int s_nblocks;	// Total number of blocks on disk
	struct File s_root;	// Root directory node
};

/* Disk block n, when in memory, is mapped into the file system
 * server's address space at DISKMAP+(n*BY2BLK). */
#define DISKMAP		0x10000000

/* Maximum disk size we can handle (1GB) */
#define DISKMAX		0x40000000

void fs_init(void);
void fs_sync(void);
extern u_int *bitmap;
int map_block(u_int);
int alloc_block(void);

#endif // _FS_H_
