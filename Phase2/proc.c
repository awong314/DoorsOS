// proc.c, 159
// all processes are coded here
// processes do not use kernel data or code, must ask via service calls

#include "spede.h"       // cons_xxx below needs
#include "kernel_data.h" // run_pid needed below
#include "syscalls.h"    // prototypes of the system calls
#include "proc.h"        // prototypes of processes

void IdleProc(void) {
   int i;
   unsigned short *p = (unsigned short *)0xb8000 + 79;//upper right corner of display 
   while(1) {
      *p = '0' + 0x0f00; // show '0' at the upper right corner
      for(i=0; i<LOOP/2; i++) asm("inb $0x80"); // delay .5 sec
      *p = ' ' + 0x0f00; // show ' ' at the upper right corner
      for(i=0; i<LOOP/2; i++) asm("inb $0x80"); // delay 
   }
}

void UserProc(void) {
   int my_pid, centi_sec;
   char str[] = "   ";
   my_pid = sys_getpid();
   centi_sec = 50 * my_pid;

   str[0] = '0' + my_pid/10;
   str[1] = '0' + my_pid%10;

   while(1) {
      sys_write(STDOUT, str, 3);	// show my PID
      sys_sleep(centi_sec);		// sleep for .5 sec x PID
   }
}
