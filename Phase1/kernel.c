// kernel.c, 159
// OS bootstrap and kernel code for OS phase 1
// OS Name: DoorsOS
// Team Name: A.W.W.W. (Members: Andrew Wong, Wesley Webb) 

#include "spede.h"         // given SPEDE stuff
#include "kernel_types.h"  // kernle data types
#include "entry.h"         // entries to kernel
#include "tools.h"         // small tool functions
#include "proc.h"          // process names such as IdleProc()
#include "services.h"      // service code

struct i386_gate *IDT_p;

// kernel data are all declared here:
int run_pid;                       // currently running PID; if -1, none selected
pid_q_t ready_pid_q, avail_pid_q;  // avail PID and those ready to run
pcb_t pcb[PROC_NUM];               // Process Control Blocks
char proc_stack[PROC_NUM][PROC_STACK_SIZE]; // process runtime stacks

void InitKernelData(void) {        // init kernel data
   int i;
   run_pid = -1;                   //initialize run_pid (to negative 1)
   MyBzero((char *)&ready_pid_q, sizeof(pid_q_t));   //clear two PID queues
   MyBzero((char *)&avail_pid_q, sizeof(pid_q_t));   
   for(i=0; i<Q_SIZE; i++) {       //enqueue all PID numbers into the available PID queue
      EnQ(i, &avail_pid_q);
   }
}

void InitKernelControl(void) {     // init kernel control
   IDT_p = get_idt_base();         //locate where IDT is
   cons_printf("IDT is located at DRAM addr %x (%d).\n", (unsigned int) IDT_p, (unsigned int) IDT_p);   //show its location on target PC
   fill_gate(&IDT_p[TIMER], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);   //call fill_gate: fill out entry TIMER with TimerEntry
   outportb(0x21, ~1);             //send PIC a mask value
}

void ProcScheduler(void) {         // choose run_pid to load/run
   if(run_pid > 0) return;         // no need if PID is a user proc

   if(ready_pid_q.size == 0)       //if the ready_pid_q is empty: let run_pid be zero
      run_pid = 0;
   else                            //else: get the 1st one in ready_pid_q to be run_pid
      run_pid = DeQ(&ready_pid_q);
   pcb[run_pid].totaltime += pcb[run_pid].runtime; //accumulate its totaltime by adding its runtime
   pcb[run_pid].runtime = 0;       //and then reset its runtime to zero
}

int main(void) {                   // OS bootstraps
   InitKernelData();               //initialize kernel data
   InitKernelControl();            //initialize kernel control

   NewProcService(IdleProc);       //call NewProcService() with address of IdleProc to create it
   ProcScheduler();                //call ProcScheduler() to select a run_pid
   ProcLoader(pcb[run_pid].trapframe_p);  //call ProcLoader() with address of the trapframe of the selected run_pid

   return 0; // compiler needs for syntax altho this statement is never exec
}

void Kernel(trapframe_t *trapframe_p) {   // kernel code runs (100 times/second)
   char key;

   pcb[run_pid].trapframe_p = trapframe_p;//save the trapframe_p to the PCB of run_pid

   TimerService();                //call TimerService() to service the timer interrupt

   if(cons_kbhit()) {             //if a key is pressed on target PC {
      key = cons_getchar();       //get the key
      if(key == 'n')              //if it's 'n,' call NewProcService() to create a UserProc
         NewProcService(UserProc);
      if(key == 'b')              //if it's 'b,' call breakpoint() to go to the GDB prompt
         breakpoint();
   }

   ProcScheduler();               //call ProcScheduler() to select run_pid
   ProcLoader(pcb[run_pid].trapframe_p);  //call ProcLoader() given the trapframe_p of the run_pid to load/run it
}

