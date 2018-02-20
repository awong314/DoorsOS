// syscalls.c
// calls to kernel services

#include "kernel_constants.h" // SYS_WRITE 4, SYS_GETPID 20, etc.

int sys_getpid(void) {
   int pid;

   asm("movl %1, %%eax;	      // service #20 (SYS_GETPID)
      int $128;	  	      // interrupt CPU with IDT entry 128
      movl %%eax, %0"	      // after, copy eax to variable 'pid'
      : "=g" (pid)	      // output syntax
      : "g" (SYS_GETPID)      // input syntax
      : "eax", "ebx" 	      // used registers
   );

   return pid;
}

 
