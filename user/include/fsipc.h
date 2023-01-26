//
// Created by yufoo1 on 2022/7/2.
//

#ifndef FIBOS_FINAL_FSIPC_H
#define FIBOS_FINAL_FSIPC_H


// Definitions for requests from clients to file system
#include "../../include/type.h"
#include "fd.h"

#define FSREQ_OPEN	1
#define FSREQ_MAP	2
#define FSREQ_SET_SIZE	3
#define FSREQ_CLOSE	4
#define FSREQ_DIRTY	5
#define FSREQ_REMOVE	6
#define FSREQ_SYNC	7

struct Fsreq_open {
    char req_path[128];
    u_int req_omode;
};

struct Fsreq_map {
    int req_fileid;
    u_int req_offset;
};

struct Fsreq_set_size {
    int req_fileid;
    u_int req_size;
};

struct Fsreq_close {
    int req_fileid;
};

struct Fsreq_dirty {
    int req_fileid;
    u_int req_offset;
};

struct Fsreq_remove {
    u_char req_path[128];
};

int fsipc_open(const char *path, u_int omode, struct Fd *fd);
int	fsipc_map(u_int, u_int, u_int);
int	fsipc_set_size(u_int, u_int);
int	fsipc_close(u_int);
int	fsipc_dirty(u_int, u_int);
int	fsipc_remove(const char *);
int	fsipc_sync(void);
int	fsipc_incref(u_int);


#endif //FIBOS_FINAL_FSIPC_H
