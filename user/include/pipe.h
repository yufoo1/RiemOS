#ifndef FIBOS_FINAL_PIPE_H
#define FIBOS_FINAL_PIPE_H

int pipe(int pfd[2]);
int pipe_is_closed(int fdnum);

#endif