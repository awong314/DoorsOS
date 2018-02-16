// syscalls.c
// calls to kernel services

#include "kernel_constants.h" // SYS_WRITE 4, SYS_GETPID 20, etc.

int sys_getpid(void) {
   int pid;

   asm("movl %1, %%eax;     // service #20 (SYS_GETPID)
        int $128;           // interrupt CPU with IDT entry 128
        movl %%ebx, %0"     // after, copy eax to variable 'pid'
       : "=g" (pid)         // output syntax
       : "g" (SYS_GETPID)   // input syntax
       : "eax", "ebx"       // used registers
    );

   return pid;
}

void sys_write(int fileno, char *str, int len) {
   asm("                    // send service #4 (SYS_WRITE) via eax
                            // send in fileno via ebx (e.g., STDOUT)
                            // send in str addr via ecx
                            // send in str len via edx
                "           // initiate service call, intr 128 (IDT entry 128)
       :                    // no output
       : "g" (...
       : "eax", ...
       );
}

void sys_sleep(int centi_sec) { // 1 centi-second is 1/100 of a second
   asm("                        // service #162 (SYS_SLEEP)
                                // send in centi-seconds via ebx
                "
       :
       : "g" (...
       : "eax", ...
       );
}

