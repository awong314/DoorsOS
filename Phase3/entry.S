// entry.S, entries to kernel services, build process trapframe

#include "kernel_constants.h"
#include <spede/machine/asmacros.h> // do not use/include "spede.h"

.comm kernel_stack, 8192, 1        // declare kernel stack space 8192 x 1 byte
.text

ENTRY(TimerEntry)
   pushl $TIMER
   jmp EnterKernel

ENTRY(SyscallEntry)
   pushl $SYSCALL
   jmp EnterKernel

// build proc frame, and go into kernel
EnterKernel:                       // will have efl, cs, eip pushed by hardware
   pusha                           //push all registers
   movl %esp, %ecx                 //copy esp to ecx
   movw $0x10, %ax                 // set kernel data segment registers
   mov %ax, %ds                    // set kernel data segment registers
   mov %ax, %es                    // set kernel data segment registers
   leal kernel_stack + 8192, %esp  // point esp to the kernel stack
   push %ecx                       //push ecx (onto the kernel stack)
   cld                             //clear the direction flag of efl
   call CNAME(Kernel)              //call the Kernel C code

// loads the trapframe of the selected process in order to run it
ENTRY(ProcLoader)
   movl 4(%esp), %eax              //copy (movl) the 4 bytes above esp into ebx
   movl %eax, %esp                 //copy ebx to esp (point esp to the process trapframe)
   popa                            //pop all registers
   add $4, %esp	                   //skip intr num 4 bytes
   iret                            //return from interrupt
