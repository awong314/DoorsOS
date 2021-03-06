#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include "kernel_constants.h"

extern int sys_getpid(void);
extern void sys_write(int, char*, int);
extern void sys_sleep(int);
extern void sys_semwait(int);
extern void sys_sempost(int);
//Phase 5 
extern void sys_read(int, char*, int);

#endif
