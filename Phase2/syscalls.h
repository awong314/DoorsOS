#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include "kernel_constants.h"

int sys_getpid(void);
void sys_write(int, char*, int);
void sys_sleep(int);

#endif
