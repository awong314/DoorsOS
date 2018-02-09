// kernel.c, 159
// OS bootstrap and kernel code for OS phase 1
// OS Name: Doors
// Team Name: A.W.W.W (Members: Wesley Webb, Andrew Wong) 

#include "spede.h"         // given SPEDE stuff
#include "kernel_types.h"  // kernle data types
#include "entry.h"         // entries to kernel
#include "tools.h"         // small tool functions
#include "proc.h"          // process names such as IdleProc()
#include "services.h"      // service code

int i;

// kernel data are all declared here:
int run_pid;                       // currently running PID; if -1, none selected
pid_q_t ready_pid_q, avail_pid_q;  // avail PID and those ready to run
pcb_t pcb[PROC_NUM];               // Process Control Blocks
char proc_stack[PROC_NUM][PROC_STACK_SIZE]; // process runtime stacks

void InitKernelData(void) {        // init kernel data
   //initialize run_pid (to negative 1)
   run_pid = -1;
   //clear two PID queues
   MyBzero(read_pid_q->q, sizeof(ready_pid_q->q));
   MyBzero(avail_pid_q->q, sizeof(avail_pid_q->q));
   //enqueue all PID numbers into the available PID queue
   for(i=0; i<Q_SIZE; i++) {   
      EnQ(i, avail_pid_q);
   }
}

void InitKernelControl(void) {     // init kernel control
   (similar to the timer lab)
   locate where IDT is
   show its location on target PC
   call fille_gate: fill out entry TIMER with TimerEntry
   send PIC a mask value
}

void ProcScheduler(void) {              // choose run_pid to load/run
   if run_pid is greater than 0, return // no need if PID is a user proc

   if the ready_pid_q is empty: let run_pid be zero
   else: get the 1st one in ready_pid_q to be run_pid

   accumulate its totaltime by adding its runtime
   and then reset its runtime to zero
}

int main(void) {  // OS bootstraps
   InitKernelData();
   InitKernelControl();	

   call NewProcService() with address of IdleProc to create it
   call ProcScheduler() to select a run_pid
   call ProcLoader() with address of the trapframe of the selected run_pid

   return 0; // compiler needs for syntax altho this statement is never exec
}

void Kernel(trapframe_t *trapframe_p) {   // kernel code runs (100 times/second)
   char key;

   save the trapframe_p to the PCB of run_pid

   call TimerService() to service the timer interrupt

   if a key is pressed on target PC {
      get the key
      if it's 'n,' call NewProcService() to create a UserProc
      if it's 'b,' call breakpoint() to go to the GDB prompt
   }

   call ProcScheduler() to select run_pid
   call ProcLoader() given the trapframe_p of the run_pid to load/run it
}

