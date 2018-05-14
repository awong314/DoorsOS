// kernel_constants.h, 159

#ifndef _KERNEL_CONSTANTS_H_
#define _KERNEL_CONSTANTS_H_

#define TIMER 32             // IDT entry #32 has code addr for timer intr (DOS IRQ0)

#define LOOP 166666         // handly loop limit exec asm("inb $0x80");
#define TIME_LIMIT 200       // max timer count, then rotate process
#define PROC_NUM 20          // max number of processes
#define Q_SIZE 20            // queuing capacity
#define PROC_STACK_SIZE 4096 // process runtime stack in bytes

// Phase 2 constants
#define SYSCALL 128	     // Entry #128 in the IDT carries the address of Syscallentry which leads to the execution of the kernel
#define STDOUT 1	  
#define SYS_WRITE 4	     
#define SYS_GETPID 20        
#define SYS_SLEEP 162	    

// Phase 3 constants
#define SYS_SEMWAIT 300
#define SYS_SEMPOST 301

// Phase 4 constants
#define TERM1 35
#define TERM2 36
#define BUFF_SIZE 101 	// Max size for term I/O buffers

// Phase 5 constants
#define SYS_READ 3
#define DSP_READY IIR_TXRDY 
#define KB_READY IIR_RXRDY

// Phase 6 constants
#define SYS_FORK 2

// Phase 7
#define SYS_SIGNAL 48         // signal service #
#define SIGINT 2              // signal ctrl-C is 2
#define SIG_NUM 32            // 32-bit OS has 32 different signals
#define SYS_PPID 64           // Parent pid 

// Phase 8 constants
#define SIGCHILD 17
#define SYS_EXIT 1
#define SYS_WAITCHILD 7

// Phase 9 constants
#define SYS_EXEC 11
#define PAGE_BASE 0xe00000    // MyOS.dli ends at byte 14M-1, 0xdfffff
#define PAGE_NUM 20           // Only 20 DRAM pages to experiment
#define PAGE_SIZE 4096        // Each DRAM page is 4KB in size

// Macro for calculating page address
#define PAGE_ADDR(x) (PAGE_BASE + x * PAGE_SIZE)
// Phase A constants
#define VM_START 0x20000000   // VM starts at addr 500MB
#define VM_END 0x5fffffff     // VM ends at addr 1.5GB
#define VM_TF ((VM_END+1)-sizeof(trapframe_t)-2*sizeof(int))   // 2G minus size of trapframe and 2 integers

#endif
