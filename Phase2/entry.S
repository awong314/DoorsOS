// entry.S, entries to kernel services, build process trapframe

#include "kernel_constants.h"
#include <spede/machine/asmacros.h> // do not use/include "spede.h"

.comm kernel_stack, 8192, 1 // declare kernel stack space 8192 x 1 byte
.text

// loads the trapframe of the selected process in order to run it
ENTRY(ProcLoader)
   movl 4(%esp), %ebx    //copy (movl) the 4 bytes above esp into ebx
   movl %ebx, %esp    //copy ebx to esp (point esp to the process trapframe)
   popa                            //pop all registers
   addl $4, %esp
   iret                            //return from interrupt

// build proc frame, and go into kernel
ENTRY(EnterKernel)                  // will have efl, cs, eip pushed by hardware
   pusha                           //push all registers
   movl %esp, %ecx                 //copy esp to ecx
   cld                             //clear direction flag in efl
   movw $0x10, %ax                 // set kernel data segment registers
   mov %ax, %ds                    // set kernel data segment registers
   mov %ax, %es                    // set kernel data segment registers
   leal kernel_stack + 8192, %esp  // point esp to the kernel stack
   push %ecx                       //push ecx (onto the kernel stack)
   cld                             //clear the direction flag of efl
   push $TIMER
   call CNAME(Kernel)              //call the Kernel C code

ENTRY(TimerEntry)
   push $TIMER
   jmp ENTRY(EnterKernel)

ENTRY(SyscallEntry)
   push $SYSCALL
   jmp ENTRY(EnterKernel)
