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

   pcb[run_pid].runtime++; 						//upcount the runtime of the running process
   if(pcb[run_pid].runtime >= TIME_LIMIT) {  //if it reaches the time limit
      pcb[run_pid].state = READY;            //change its state to READY
      EnQ(run_pid, &ready_pid_q);            //queue it to ready_pid_q
      run_pid = -1;                          //reset run_pid (to -1)
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
		case SYS_SEMWAIT:
         SemwaitService((int)p->ebx);
         break;
		case SYS_SEMPOST:
         SempostService((int)p->ebx);
         break;
		default:
			cons_printf("Error due to p->eax being %d",p->eax);
			break;			
	}
}

void GetpidService(int *p) {
	*p = run_pid;
}

void SleepService(int centi_sec) {
   pcb[run_pid].wake_time = current_time + centi_sec;
   pcb[run_pid].state = SLEEP;
   run_pid = -1;	
}

void WriteService(int fileno, char *str, int len) {
   static unsigned short *vga_p = (unsigned short *)0xb8000;
<<<<<<< HEAD
   int i=0, which = fileno % 2;

   // Phase 2 & 3 writing to STDOUT
=======
   int w = 0;
/* Phase 2/3
>>>>>>> e1f057f8d12bc6d78ddc9a83850c1f0e4a5a3b1b
   if(fileno == STDOUT) {
		for(i=0; i<len; i++) {
      	*vga_p = str[i] + 0xf00;
	 		vga_p++;
	 		if(vga_p >= (unsigned short *)0xb8000 + 25*80) {
         	int j;
	    		for(j=0; j<25; j++)
	    		cons_printf("\n"); 
	    		vga_p = (unsigned short *)0xb8000;	 
	 		}
      }
<<<<<<< HEAD
   } 
   // Phase 4
   else if (fileno == TERM1) {
      for(i=0; i<len; i++) {
         term[which].dsp[i] = *str;
         str++;
      }
		pcb[run_pid].state = WAIT; 
		EnQ(run_pid, &term[which].dsp_wait_q);
=======
   } else 
*/
   if (fileno == TERM1) {
		
>>>>>>> e1f057f8d12bc6d78ddc9a83850c1f0e4a5a3b1b
	} else if (fileno == TERM2) {
		for(i=0; i<len; i++) {
         term[which].dsp[i] = *str;
         str++;
      }
		pcb[run_pid].state = WAIT;
		EnQ(run_pid, &term[which].dsp_wait_q);
	}
}

void SemwaitService(int sem_num) {
   if(sem_num == STDOUT) {
      if(video_sem.val > 0) {
         video_sem.val--;
      }   
      else {
         // ---> "block" the running process:
         EnQ(run_pid, &video_sem.wait_q); // 1. enqueue it to the wait queue in the semaphore
         pcb[run_pid].state = WAIT;	   	// 2. change its state
         run_pid = -1;			   			// 3. no running process anymore (lack one)        
      }
   }
   else {
      cons_printf("Kernel Panic: non-such semaphore number!\n");
   }
}

void SempostService(int sem_num) {
   int pid;
   if(sem_num == STDOUT) {
      if(video_sem.wait_q.size == 0) 
         video_sem.val++;
      else {
			// ---> "liberate" the process
         pid = DeQ(&video_sem.wait_q);//   1. dequeue it from the wait queue in the semaphore 
         pcb[pid].state = READY;      //   2. change its state
         EnQ(pid, &ready_pid_q);      //   3. enqueue the liberated process (ID) to the ready PID queue
      }    
   }
   else { 
      cons_printf("Kernel Panic: non-such semaphore number!\n");
   }
   printf("video_sem: %d",video_sem.val);
}

void TermService(int which) {
	int i, pid;

	if(term[which].dsp[0] == (char *)0) return; // if 1st character of dsp buffer is null, return; // nothing to dsp
	
	outportb(term[which].port, term[which].dsp[0]); // disp 1st char

	for(i=1; i<BUFF_SIZE; i++) {			            //conduct a loop, one by one 
		term[which].dsp[i-1] = term[which].dsp[i];	//move each character in dsp buffer forward by 1 character
	   //if encounter moving a NULL character, break loop; How will this work? Don't we need to move everything in the buffer forward?
	}

   if(term[which].dsp[0] = (char *)0) {
      
   }

	if 1st char of dsp buffer is null and the wait queue has PID {
		// str ends & there's a waiter
		// release the 1st waiter in the wait queue:
		1. dequeue it from the wait queue
		2. update its state
		3. enqueue it to ready PID queue
	}
}

