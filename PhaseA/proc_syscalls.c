// syscalls.c
// calls to kernel services

#include "kernel_constants.h" // SYS_WRITE 4, SYS_GETPID 20, etc.
#include "kernel_types.h"

int sys_getpid(void) {
   int pid;

   asm("movl %1, %%eax;       // service #20 (SYS_GETPID)
      int $128;	            // interrupt CPU with IDT entry 128
      movl %%ebx, %0;"	      // after, copy ebx to variable 'pid'
      : "=g" (pid)	         // output syntax
      : "g" (SYS_GETPID)      // input syntax
      : "eax", "ebx" 	      // used registers
   );

   return pid;
}

void sys_write(int fileno, char *str, int len) {
   if(*str == '\0') return;

   asm("movl %0, %%eax;       // service #4 (SYS_WRITE) via eax
      movl %1, %%ebx;	      // send in fileno via ebx (e.g., STDOUT)
      movl %2, %%ecx;         // send in str addr via ecx
      movl %3, %%edx;         // send in str length via edx
      int $128;" 	            // initiate service call, intr 128 (IDT entry 128)
      : 
      : "g" (SYS_WRITE), "g" (fileno), "g" ((int)str), "g" (len)
      : "eax", "ebx", "ecx", "edx"
   ); 
} 

//Phase 5
void sys_read(int fileno, char *str, int len) {
   if(len==0) return;

   asm("movl %0, %%eax;       // service #3 (SYS_READ) via eax
      movl %1, %%ebx;	      // send in fileno via ebx (e.g., STDOUT)
      movl %2, %%ecx;         // send in str addr via ecx
      movl %3, %%edx;         // send in str length via edx
      int $128;" 	            // initiate service call, intr 128 (IDT entry 128)
      : 
      : "g" (SYS_READ), "g" (fileno), "g" ((int)str), "g" (len)
      : "eax", "ebx", "ecx", "edx"
   ); 
} 

void sys_sleep(int centi_sec) {	// 1 centi-second is 1/100 of a second
   asm("movl %0, %%eax;       // Service #162 SLEEP
      movl %1, %%ebx;	      // send in centi-seconds via ebx
      int $128;"
      :
      : "g" (SYS_SLEEP), "g" (centi_sec)
      : "eax", "ebx"
   );
}

void sys_semwait(int sem_num) {
  asm("movl %0, %%eax;        // Service #300 SEMWAIT
      movl %1, %%ebx;	      // send in sem_num via ebx
      int $128;"
      :
      : "g" (SYS_SEMWAIT), "g" (sem_num)
      : "eax", "ebx"
   );
}

void sys_sempost(int sem_num) {
   asm("movl %0, %%eax;       // Service #301 SEMPOST
      movl %1, %%ebx;	      // send in sem_num via ebx
      int $128;"
      :
      : "g" (SYS_SEMPOST), "g" (sem_num)
      : "eax", "ebx"
   );
} 

//Phase 6
int sys_fork(void) {
   int pid;

   asm("movl %1, %%eax;       // Service #2 Fork
      int $128;
      movl %%ebx, %0;"	      // after, copy ebx to variable 'pid'
      : "=g" (pid)
      : "g" (SYS_FORK)
      : "eax", "ebx"
   );

   return pid;
}

//Phase 7
int sys_getppid() {
   int pid;

   asm("movl %1, %%eax;       
      int $128;	           
      movl %%ebx, %0;"	      
      : "=g" (pid)	         
      : "g" (SYS_PPID)      
      : "eax", "ebx" 	      
   );
   return pid;
}

void sys_signal(int signal, func_p_t p) {
  asm("movl %0, %%eax;        // Service #48 Signal
      movl %1, %%ebx;	      // send in signal via ebx 
      movl %2, %%ecx;         // send in p addr via ecx
      int $128;"
      :
      : "g" (SYS_SIGNAL), "g" (signal), "g" ((int)p)
      : "eax", "ebx", "ecx"
   ); 
}

// PHASE 8
void sys_exit(int exit_code) {
   asm("movl %0, %%eax;
      movl %1, %%ebx;         // put exit_code to ebx
      int $128;"         
      :
      :  "g" (SYS_EXIT), "g" (exit_code)
      :  "eax", "ebx"
   );
}

int sys_waitchild(int *exit_code_p) {
   int child_pid;
   asm(" movl %1, %%eax;
      movl %2, %%ebx;
      int $128;
      movl %%ecx, %0"   // copy ecx to child_pid
      :  "=g" (child_pid)
      :  "g" (SYS_WAITCHILD), "g" ((int)exit_code_p)
      :  "eax", "ebx", "ecx"
   );
   return child_pid;
}

// Phase 9
void sys_exec(func_p_t p, int arg) {

   asm("movl %0, %%eax;        
      movl %1, %%ebx;	      // send in p addr via ebx 
      movl %2, %%ecx;         // send in arg via ecx
      int $128;"
      :
      : "g" (SYS_EXEC), "g" ((int)p), "g" (arg)
      : "eax", "ebx", "ecx"
   );
}





