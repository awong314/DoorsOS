// proc.c, 159
// all processes are coded here
// processes do not use kernel data or code, must ask via service calls

#include "spede.h"       // cons_xxx below needs
#include "kernel_data.h" // run_pid needed below
#include "proc.h"        // prototypes of processes

void IdleProc(void) {
   int i;

   while(1) {
      cons_printf("0 ");    // SystemProc has PID 0
      for(i=0; i<LOOP; i++) {
         asm("inb $0x80");  // to cause delay approx 1 second
      }
   }
}

void UserProc(void) {
   int i;

   while(1) {
      //show on target PC own PID (run_pid) using cons_printf()
      cons_printf("%d", run_pid);
      //repeat LOOP times: call asm("inb $0x80")  // to cause delay approx 1 second
      for(i=0; i<LOOP; i++) {
         asm("inb $0x80");
      }
   }
}
