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
int run_pid;                       				// currently running PID; if -1, none selected
pid_q_t ready_pid_q, avail_pid_q, page_q;		// avail PID and those ready to run
pcb_t pcb[PROC_NUM];               				// Process Control Blocks
char proc_stack[PROC_NUM][PROC_STACK_SIZE];  // process runtime stacks
int current_time;
semaphore_t video_sem;								// Phase 4
term_t term[2];										// Phase 4
func_p_t signal_table[PROC_NUM][SIG_NUM];    // Phase 7
int OS_TT;                                   // Phase A

// Phase 4 (Given)
void InitTerm(void) {
	int i, j;

	for(j=0; j<2; j++) {
		outportb(term[j].port + CFCR, CFCR_DLAB);
		outportb(term[j].port + BAUDLO, LOBYTE(115200/9600));
		outportb(term[j].port + BAUDHI, HIBYTE(115200/9600));
		outportb(term[j].port + CFCR, CFCR_PEVEN | CFCR_PENAB | CFCR_7BITS);

		outportb(term[j].port + IER, 0);
		outportb(term[j].port + MCR, MCR_DTR|MCR_RTS|MCR_IENABLE);
		outportb(term[j].port + IER, IER_ERXRDY|IER_ETXRDY);

		for(i=0;i<LOOP;i++) asm("inb $0x80");

		inportb(term[j].port);		   
	}
}

void InitKernelData(void) {        
   int i;
   run_pid = -1;                   						//initialize run_pid (to negative 1)
	// Clear kernel data
   MyBzero((char *)&ready_pid_q, sizeof(pid_q_t)); // Phase 1  
   MyBzero((char *)&avail_pid_q, sizeof(pid_q_t));	// Phase 4
   MyBzero((char *)&video_sem.wait_q, sizeof(pid_q_t));  // Phase 3
   MyBzero((char *)&page_q, sizeof(pid_q_t));      // Phase 9
   MyBzero((char *)PAGE_BASE, PAGE_SIZE*PAGE_NUM); // Phase 9
   MyBzero((char *)&term[0], sizeof(term_t));      // Phase 4
   MyBzero((char *)&term[1], sizeof(term_t));      // Phase 4
   term[0].port = 0x2f8;			                  // Phase 4
   term[1].port = 0x3e8;			                  // Phase 4
   // Phase 5
   term[0].status = 0x2f8 + IIR;                   // intr indicator reg
   term[1].status = 0x3e8 + IIR;
   // Phase 7 (clear the signal table)
   MyBzero((char *)&signal_table, sizeof(func_p_t)*SIG_NUM*PROC_NUM);
   OS_TT = get_cr3();
   for(i=0; i<Q_SIZE; i++) {       						//enqueue all PID numbers
      EnQ(i, &avail_pid_q);
   }
   // Phase 9
   for(i=0; i<PAGE_NUM; i++) {       					//enqueue all page numbers
      EnQ(i, &page_q);
   }
}

void InitKernelControl(void) {     // init kernel control
   IDT_p = get_idt_base();         // locate where IDT is
   cons_printf("IDT is located at DRAM addr %x (%d).\n", (unsigned int) IDT_p, (unsigned int) IDT_p);   //show its location on target PC
   fill_gate(&IDT_p[TIMER], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
   fill_gate(&IDT_p[SYSCALL], (int)SyscallEntry, get_cs(), ACC_INTR_GATE, 0);   //call fill_gate: fill out entry TIMER with TimerEntry
   fill_gate(&IDT_p[TERM1], (int)Term1Entry, get_cs(), ACC_INTR_GATE, 0);
   fill_gate(&IDT_p[TERM2], (int)Term2Entry, get_cs(), ACC_INTR_GATE, 0);   
   outportb(0x21, ~(0x19));        //send PIC a mask value
}
   
void ProcScheduler(void) {         // choose run_pid to load/run
   if(run_pid > 0) return;         // no need if PID is a user proc

   if(ready_pid_q.size == 0)       // if the ready_pid_q is empty: let run_pid be zero
      run_pid = 0;
   else                            // else: get the 1st one in ready_pid_q to be run_pid
      run_pid = DeQ(&ready_pid_q);
   pcb[run_pid].totaltime += pcb[run_pid].runtime; 
   pcb[run_pid].runtime = 0;   
}

int main(void) {                    // OS bootstraps
   current_time = 0;
   video_sem.val = 1;
   InitKernelData();                // initialize kernel data
   InitKernelControl();             // initialize kernel control
   InitTerm();
   NewProcService(IdleProc);        // call NewProcService() with address of IdleProc to create it 
   ProcScheduler();                 // call ProcScheduler() to select a run_pid
   ProcLoader(pcb[run_pid].trapframe_p);// call ProcLoader() with address of the trapframe of the selected run_pid
   return 0;                        // compiler needs for syntax altho this statement is never exec
}

void Kernel(trapframe_t *trapframe_p) {   // kernel code runs (100 times/second)
   char key;
   pcb[run_pid].trapframe_p = trapframe_p;// save the trapframe_p to the PCB of run_pid 
   switch(trapframe_p->intr_num) {
      case TIMER:
         TimerService();
         break;
      case SYSCALL:
         SyscallService(trapframe_p);
         break;
      case TERM1:
         TermService(0);
         outportb(0x20,0x63); 
         break;
      case TERM2:
         TermService(1); 
         outportb(0x20,0x64);
         break;
      default:
         cons_printf("Invalid intr");
         break;
   }
   if(cons_kbhit()) {             // if a key is pressed on target PC {
      key = cons_getchar();       // get the key
      if(key == 'n')  
         NewProcService(UserProc);
      if(key == 'b')     
         breakpoint();
   }

   ProcScheduler();                       // call ProcScheduler() to select run_pid
   // Phase A 
   if(pcb[run_pid].TT != 0) {             // If TT is set then give this TT to the MMU
      set_cr3(pcb[run_pid].TT);
   }
   ProcLoader(pcb[run_pid].trapframe_p);  // call ProcLoader() given the trapframe_p of the run_pid to load/run it
}


