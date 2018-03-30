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
void NewProcService(func_p_t proc_p) {    // arg: where process code starts
   int pid;
   if(avail_pid_q.size == 0) {            //if the size of avail_pid_q is 0 may occur as too many proc got created
      cons_printf("\nKERNEL PANIC: Plz No more clik clak"); 
      return;                             // alternative: breakpoint()
   }

   pid = DeQ(&avail_pid_q);               //get a 'pid' from avail_pid_q
   MyBzero((char *)&pcb[pid], sizeof(pcb_t)); //use tool MyBzero() to clear PCB and proc stack
   MyBzero((char *)&proc_stack[pid][0], PROC_STACK_SIZE);
   pcb[pid].state = READY;                //set its process state to READY
   if(pid != 0) EnQ(pid, &ready_pid_q);   //queue pid to be ready_pid_q unless it's 0 (IdleProc)

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
      //Phase 5
      case SYS_READ:
         ReadService((int)p->ebx,(char *)p->ecx,(int)p->edx);  
         break;
      //Phase 6
      case SYS_FORK:
         ForkService(&p->ebx);
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
   int i; 
   int which = (fileno+1) % 2;

   // Phase 2 & 3 writing to STDOUT
   if(fileno == STDOUT) {
      for(i=0; i<len; i++) {
         *vga_p = str[i] + 0xf00;
         vga_p++;
         if(vga_p >= (unsigned short *)0xb8000 + 25*80) {
            // Use tools to clear screen starting at index top left of screen
            MyBzero((char*)0xb8000, 25*80*2);
            // Reset video mem pointer to top left
            vga_p = (unsigned short *)0xb8000;	 
         } 
      }
   }
   // Phase 4
   else if (fileno == TERM1 || fileno == TERM2) {
      MyStrcpy(term[which].dsp, str); 
      EnQ(run_pid, &term[which].dsp_wait_q);
      pcb[run_pid].state = WAIT;
      run_pid = -1; 
      DspService(which); //Changed from TermService(which) in Phase 4 
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

//Phase 5 modified from Phase 4
void TermService(int which) {
   int checker = inportb(term[which].status);
   switch(checker) {
      case DSP_READY:
         DspService(which);
         break;
      case KB_READY:
         KbService(which);
         break;
   }
}

//Same as TermService from Phase 4 so far
void DspService(int which) {
   int pid;

   if(term[which].dsp[0] == '\0') return; // if 1st char of dsp = null return; nothing to dsp

   outportb(term[which].port, term[which].dsp[0]); // disp 1st char

   MyStrShift(term[which].dsp);

   if(term[which].dsp[0] == '\0' && (term[which].dsp_wait_q.size != 0)) {
      // str ends & there's a waiter
      // release the 1st waiter in the wait queue:
      //1. dequeue it from the wait queue
      //2. update its state
      //3. enqueue it to ready PID queue
      pid = DeQ(&term[which].dsp_wait_q);
      pcb[pid].state = READY;
      EnQ(pid, &ready_pid_q);
   }
}

void KbService(int which) {
   int pid;
   char ch;
   // Read a character from the 'port' of the terminal
   ch = inportb(term[which].port);
   // Also write it out via the 'port' of the terminal (to echo back)
   outportb(term[which].port, ch);
   
   // If what's read is NOT a '\r' (CR) key, append it to kb[] string
   if(ch != '\r') {
      MyStrAppend(term[which].kb, ch);
      return;
   }

   // (not returning, continue) if there appears a waiting process in
   //  the kb wait queue of the terminal, release it and feed it the
   //  kb str it needs (use MyStrcpy)
   if(term[which].kb_wait_q.size > 0) {
      pid = DeQ(&term[which].kb_wait_q);
      pcb[pid].state = READY;
      EnQ(pid, &ready_pid_q);
      MyStrcpy((char *)pcb[pid].trapframe_p->ecx,term[which].kb);  
      // Reset the terminal kb string (put a single NUL at its start)
      term[which].kb[0] = '\0'; 
   }
}

void ReadService(int fileno, char *str, int len) {
   //Choose which term to use
   int which = (fileno+1) % 2;

   //Block running proc to chosen term
   EnQ(run_pid, &term[which].kb_wait_q);
   pcb[run_pid].state = WAIT;
   run_pid = -1;
}

//Phase 6
void ForkService(int *ebx_p) {
   int pid;

   if(avail_pid_q.size == 0) {            //No PID's left
      *ebx_p = -1;
      cons_printf("No PID's left!!!\n");
      return;
   }

   //Grab new PID for child
   *ebx_p = DeQ(avail_pid_q);
   //Enqueue the proc                
   EnQ(*ebx_p, read_pid_q);   
   
   //Init the child PCB
   MyBzero(pcb[*ebx_p], sizeof(pcb_t));   //Clear PCB of child
   pcb[*ebx_p].state = READY;             
   pcb[*ebx_p].ppid = run_pid;            //Set parent PID to the current running proc

   //Duplicate parent's runtime stack to child's stack
   

   //set the ebx in the child's trapframe to 0 (so the syscall will return 0 for the child process)

   //Calculate the address difference between the two stacks, and
   //Apply it to these in child's trapframe: esp, ebp, esi, edi

      (the following is the treatment for address changes of copied local
      variables such as 'a_pid' which apperas in both parent and child)
      about the ebp register in the child's trapframe:
         set an integer pointer p to the value what ebp points to
      loop on the condition that: the value what p points to is not 0:
         adjust what it points to with the address difference, and
         set p to this newly adjusted address, and
         loop again
}
