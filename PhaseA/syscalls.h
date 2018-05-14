#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include "kernel_constants.h"

extern int sys_getpid(void);
extern void sys_write(int, char*, int);
extern void sys_sleep(int);
extern void sys_semwait(int);
extern void sys_sempost(int);
// Phase 5 
extern void sys_read(int, char*, int);
// Phase 6
extern int sys_fork(void);
// Phase 7
extern void sys_signal(int, func_p_t);
extern int sys_getppid(void);
// Phase 8
extern void sys_exit(int exit_code);
extern int sys_waitchild(int *exit_code_p);
// Phase 9
extern void sys_exec(func_p_t p, int arg);

#endif
