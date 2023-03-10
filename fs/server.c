/*
 * File system server main loop -
 * serves IPC requests from other environments.
 */

#include "../include/memory.h"
#include "../user/include/syscallLib.h"
#include "../user/include/uLib.h"
#include "../../include/error.h"
#include "../user/include/ipc.h"
#include "../user/include/fsipc.h"

struct Open {
	struct File *o_file;	// mapped descriptor for open file
	u_int o_fileid;		// file id
	int o_mode;		// open mode
	struct Filefd *o_ff;	// va of filefd page
};

// Max number of open files in the file system at once
#define MAXOPEN			1024
#define FILEVA 			0x60000000

// initialize to force into data section
struct Open opentab[MAXOPEN] = { { 0, 0, 1 } };

// Virtual address at which to receive page mappings containing client requests.
#define REQVA	0x0ffff000

// Overview:
//	Initialize file system server process.
void
serve_init(void)
{
	int i;
	u_int va;

	// Set virtual address to map.
	va = FILEVA;

	// Initial array opentab.
	for (i = 0; i < MAXOPEN; i++) {
		opentab[i].o_fileid = i;
		opentab[i].o_ff = (struct Filefd *)va;
		va += PAGE_SIZE;
	}
}

// Overview:
//	Allocate an open file.
int
open_alloc(struct Open **o)
{
	int i, r;

	// Find an available open-file table entry
	for (i = 0; i < MAXOPEN; i++) {
		switch (getPhysicalPageRef(opentab[i].o_ff)) {
			case 0:
				if ((r = memoryAlloc(0, (u_int)opentab[i].o_ff,
										   PTE_V | PTE_R | PTE_G)) < 0) {
					return r;
				}
			case 1:
				opentab[i].o_fileid += MAXOPEN;
				*o = &opentab[i];
				memset((void *)opentab[i].o_ff, 0, PAGE_SIZE);
				return (*o)->o_fileid;
		}
	}

	return -E_MAX_OPEN;
}

// Overview:
//	Look up an open file for envid.
int
open_lookup(u_int envid, u_int fileid, struct Open **po)
{
	struct Open *o;

	o = &opentab[fileid % MAXOPEN];

	if (getPhysicalPageRef(o->o_ff) == 1 || o->o_fileid != fileid) {
		return -E_INVAL;
	}

	*po = o;
	return 0;
}

// Serve requests, sending responses back to envid.
// To send a result back, ipc_send(envid, r, 0, 0).
// To include a page, ipc_send(envid, r, srcva, perm).

void
serve_open(u_int envid, struct Fsreq_open *rq)
{
	printf("serve_open %08x %x 0x%x\n", envid, (int)rq->req_path, rq->req_omode);

	u_char path[128];
	struct File *f;
	struct Filefd *ff;
	int fileid;
	int r;
	struct Open *o;

	// Copy in the path, making sure it's null-terminated
	memcpy(rq->req_path, path, 128);
	path[128 - 1] = 0;

	// Find a file id.
	if ((r = open_alloc(&o)) < 0) {
//		panic("open_alloc failed: %d, invalid path: %s", r, path);
		ipc_send(envid, r, 0, 0);
	}

	fileid = r;

	// Open the file.
	if ((r = file_open((char *)path, &f)) < 0) {
        printf("file_open failed: %d, invalid path: %s", r, path);
        while(1);
	//	user_panic("file_open failed: %d, invalid path: %s", r, path);
		ipc_send(envid, r, 0, 0);
		return ;
	}

	// Save the file pointer.
	o->o_file = f;

	// Fill out the Filefd structure
	ff = (struct Filefd *)o->o_ff;
	ff->f_file = *f;
	ff->f_fileid = o->o_fileid;
	o->o_mode = rq->req_omode;
	ff->f_fd.fd_omode = o->o_mode;
	ff->f_fd.fd_dev_id = devfile.dev_id;

	ipc_send(envid, 0, (u_int)o->o_ff, PTE_V | PTE_R | PTE_G);
}

void
serve_map(u_int envid, struct Fsreq_map *rq)
{

	struct Open *pOpen;

	u_int filebno;

	void *blk;

	int r;

	if ((r = open_lookup(envid, rq->req_fileid, &pOpen)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

	filebno = rq->req_offset / BY2BLK;

	if ((r = file_get_block(pOpen->o_file, filebno, &blk)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

	ipc_send(envid, 0, (u_int)blk, PTE_V | PTE_R | PTE_G);
}

void
serve_set_size(u_int envid, struct Fsreq_set_size *rq)
{
	struct Open *pOpen;
	int r;
	if ((r = open_lookup(envid, rq->req_fileid, &pOpen)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

	if ((r = file_set_size(pOpen->o_file, rq->req_size)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

	ipc_send(envid, 0, 0, 0);
}

void
serve_close(u_int envid, struct Fsreq_close *rq)
{
	struct Open *pOpen;

	int r;

	if ((r = open_lookup(envid, rq->req_fileid, &pOpen)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

//	file_close(pOpen->o_file);
	ipc_send(envid, 0, 0, 0);
}

// Overview:
//	fs service used to delete a file according path in `rq`.
/*** exercise 5.10 ***/
void
serve_remove(u_int envid, struct Fsreq_remove *rq)
{
	int r;
	u_char path[MAXPATHLEN];

	// Step 1: Copy in the path, making sure it's terminated.
	// Notice: add \0 to the tail of the path
	memcpy(rq->req_path, path, MAXPATHLEN);
	path[MAXPATHLEN - 1] = '\0';
	// Step 2: Remove file from file system and response to user-level process.
	// Call file_remove and ipc_send an approprite value to corresponding env.
	r = file_remove(path);
	ipc_send(envid, r, 0, 0);
}

void
serve_dirty(u_int envid, struct Fsreq_dirty *rq)
{

	// Your code here
	struct Open *pOpen;
	int r;

	if ((r = open_lookup(envid, rq->req_fileid, &pOpen)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

	if ((r = file_dirty(pOpen->o_file, rq->req_offset)) < 0) {
		ipc_send(envid, r, 0, 0);
		return;
	}

	ipc_send(envid, 0, 0, 0);
}

void
serve_sync(u_int envid)
{
	fs_sync();
	ipc_send(envid, 0, 0, 0);
}

void
serve(void)
{
	u_int req, whom, perm;

	for (;;) {
		perm = 0;

		req = ipc_recv(&whom, REQVA, &perm);


		// All requests must contain an argument page
		if (!(perm & PTE_V)) {
			printf("Invalid request from %08x: no argument page\n", whom);
			continue; // just leave it hanging, waiting for the next request.
		}

		switch (req) {
			case FSREQ_OPEN:
				serve_open(whom, (struct Fsreq_open *)REQVA);
				break;

			case FSREQ_MAP:
				serve_map(whom, (struct Fsreq_map *)REQVA);
				break;

			case FSREQ_SET_SIZE:
				serve_set_size(whom, (struct Fsreq_set_size *)REQVA);
				break;

			case FSREQ_CLOSE:
				serve_close(whom, (struct Fsreq_close *)REQVA);
				break;

			case FSREQ_DIRTY:
				serve_dirty(whom, (struct Fsreq_dirty *)REQVA);
				break;

			case FSREQ_REMOVE:
				serve_remove(whom, (struct Fsreq_remove *)REQVA);
				break;

			case FSREQ_SYNC:
				serve_sync(whom);
				break;

			default:
				printf("Invalid request code %d from %08x\n", whom, req);
				break;
		}

		unmapMemoryFrom(0, REQVA);
	}
}

int userMain(int argc, char **argv)
{
	printf("FS is running\n");
	printf("FS can do I/O\n");
    printf("start init serve\n");
	serve_init();
    printf("start init fs\n");
	fs_init();
    printf("start serve\n");
	serve();
    return 0;
}

