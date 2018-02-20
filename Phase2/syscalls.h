#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include "kernel_constants.h"

extern int sys_getpid(void);
extern void sys_write(int, char*, int);
extern void sys_sleep(int);

#endif
