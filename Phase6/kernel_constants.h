// kernel_constants.h, 159

#ifndef _KERNEL_CONSTANTS_H_
#define _KERNEL_CONSTANTS_H_

#define TIMER 32             // IDT entry #32 has code addr for timer intr (DOS IRQ0)

#define LOOP 1666666         // handly loop limit exec asm("inb $0x80");
#define TIME_LIMIT 200       // max timer count, then rotate process
#define PROC_NUM 20          // max number of processes
#define Q_SIZE 20            // queuing capacity
#define PROC_STACK_SIZE 4096 // process runtime stack in bytes

//Phase2 constants
#define SYSCALL 128	     // Entry #128 in the IDT carries the address of Syscallentry which leads to the execution of the kernel
#define STDOUT 1	  
#define SYS_WRITE 4	     
#define SYS_GETPID 20        
#define SYS_SLEEP 162	    

//Phase3 constants
#define SYS_SEMWAIT 300
#define SYS_SEMPOST 301

//Phase4 constants
#define TERM1 35
#define TERM2 36
#define BUFF_SIZE 101 	// Max size for term I/O buffers

//Phase5 constants
#define SYS_READ 3
#define DSP_READY IIR_TXRDY 
#define KB_READY IIR_RXRDY

//Phase6 constants
#define SYS_FORK 2

#endif
