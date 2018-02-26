// services.c, 159

#include "spede.h"
#include "kernel_types.h"
#include "kernel_data.h" 
#include "services.h"
#include "tools.h"       
#include "proc.h"

int count=0;

// to create process, alloc PID, PCB, and process stack
// build trapframe, initialize PCB, record PID to ready_pid_q (unless 0)
void NewProcService(func_p_t proc_p) {  // arg: where process code starts
   int pid;
   if(avail_pid_q.size == 0) {          //if the size of avail_pid_q is 0 may occur as too many proc got created
      cons_printf("\nKERNEL PANIC: Plz No more clik clak"); //show on target PC: "Kernel Panic: no more process!\n"
      return;                   // alternative: breakpoint()
   }

   pid = DeQ(&avail_pid_q);     //get a 'pid' from avail_pid_q
   MyBzero((char *)&pcb[pid], sizeof(pcb_t)); //use tool MyBzero() to clear PCB and proc stack
   MyBzero((char *)&proc_stack[pid][0], PROC_STACK_SIZE);
   pcb[pid].state = READY;      //set its process state to READY
   if(pid != 0) EnQ(pid, &ready_pid_q);  //queue pid to be ready_pid_q unless it's 0 (IdleProc)

   pcb[pid].trapframe_p = (trapframe_t *)&proc_stack[pid][PROC_STACK_SIZE - sizeof(trapframe_t)]; //point its trapframe_p into its stack (to create the process trapframe)
   pcb[pid].trapframe_p->efl = EF_DEFAULT_VALUE | EF_INTR; //fill out efl with "EF_DEFAULT_VALUE | EF_INTR" // to enable intr!
   pcb[pid].trapframe_p->eip = (int)proc_p;  //fill out eip to proc_p
   pcb[pid].trapframe_p->cs = get_cs();      //fill out cs with the return of get_cs() call
}

// count runtime of process and preempt it when reaching time limit
void TimerService(void) {  
   int k = 0;
   current_time++;
   for(k=0; k<PROC_NUM; k++) {
      if(pcb[k].state == SLEEP && pcb[k].wake_time == current_time) {
         EnQ(k,&ready_pid_q);
	 pcb[k].state = READY;
      }
   }
   
   outportb(0x20, 0x60); //dismiss timer (IRQ0)

   if(run_pid == 0) return; //if the run_pid is 0, simply return (IdleProc is exempted)

   pcb[run_pid].runtime++; //upcount the runtime of the running process
   if(pcb[run_pid].runtime >= TIME_LIMIT) { //if it reaches the time limit
      pcb[run_pid].state = READY;           //change its state to READY
      EnQ(run_pid, &ready_pid_q);           //queue it to ready_pid_q
      run_pid = -1;                         //reset run_pid (to -1)
   }
}

void SyscallService(trapframe_t *p) {

	switch(p->eax) {
		case SYS_GETPID:
			GetpidService(&p->ebx);
			break;
		case SYS_SLEEP: 
			SleepService((int)p->ebx);
			break;
		case SYS_WRITE:
			WriteService((int)p->ebx,(char *)p->ecx,(int)p->edx);
			break;
		default:
			cons_printf("Error due to p->eax being %d",p->eax);
			break;			
	}
}

void GetpidService(int *p)
{
	*p = run_pid;
}

void SleepService(int centi_sec) {

   pcb[run_pid].wake_time = current_time + centi_sec;
   pcb[run_pid].state = SLEEP;
   run_pid = -1;	
}

void WriteService(int fileno, char *str, int len) {
   static unsigned short *vga_p = (unsigned short *)0xb8000;
   int w = 0;

   if(fileno == STDOUT) {
      //while(*str != (char *)0) {
      for(w = 0; w<len; w++) {
         *vga_p = str[w] + 0xf00;
	 vga_p++;
	 //str++;
	 if(vga_p >= (unsigned short *)0xb8000 + 25*80) {
            int j;
	    for(j=0; j<25; j++)
	    	cons_printf("\n"); 
	    vga_p = (unsigned short *)0xb8000;	 
	 }
      }
   }

}

