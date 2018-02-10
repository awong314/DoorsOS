// services.c, 159

#include "spede.h"
#include "kernel_types.h"
#include "kernel_data.h" 
#include "services.h"
#include "tools.h"       
#include "proc.h"

// to create process, alloc PID, PCB, and process stack
// build trapframe, initialize PCB, record PID to ready_pid_q (unless 0)
void NewProcService(func_p_t proc_p) {  // arg: where process code starts
   int pid;

   if (avail_pid_q.size == 0) { // may occur as too many proc got created
      cons_printf("Kernel Panic: no more process!\n");
      return;                   // alternative: breakpoint()
   }

   //get a 'pid' from avail_pid_q
   pid = avail_pid_q.q[run_pid];
   //use tool MyBzero() to clear PCB and proc stack
   MyBzero((char *)pcb,sizeof(pcb));
   //set its process state to READY
   pcb[run_pid].state = READY;
   //queue pid to be ready_pid_q unless it's 0 (IdleProc)
   

   //point its trapframe_p into its stack (to create the process trapframe)
   pcb[run_pid].trapframe_p = pcb[run_pid];
   //fill out efl with "EF_DEFAULT_VALUE | EF_INTR" // to enable intr!
   pcb[run_pid].efl = EF_DEFAULT_VALUE | EF_INTR; 
   //fill out eip to proc_p
   pcb[run_pid].eip = proc_p;
   //fill out cs with the return of get_cs() call
   pcb[run_pid].cs = get_cs();
}

// count runtime of process and preempt it when reaching time limit
void TimerService(void) {

   //dismiss timer (IRQ0)
   
   //(every .75 second display a dot symbol '.')

   //if the run_pid is 0, simply return (IdleProc is exempted)

   //upcount the runtime of the running process
   //if it reaches the time limit
      //change its state to READY
      //queue it to ready_pid_q
      //reset run_pid (to -1)
   //}
}

