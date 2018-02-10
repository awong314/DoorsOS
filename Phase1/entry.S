// entry.S, entries to kernel services, build process trapframe

#include "kernel_constants.h"
#include <spede/machine/asmacros.h> // do not use/include "spede.h"

.comm kernel_stack, 8192, 1 // declare kernel stack space 8192 x 1 byte
.text

// loads the trapframe of the selected process in order to run it
ENTRY(ProcLoader)
   //copy (movl) the 4 bytes above esp into ebx
   movl 4(%esp), %ebx
   //copy ebx to esp (point esp to the process trapframe)
   mov %ebx, %esp
   //pop all registers
   popa
   //return from interrupt
   iret

// build proc frame, and go into kernel
ENTRY(TimerEntry)                  // will have efl, cs, eip pushed by hardware
   //push all registers
   pusha
   //copy esp to ecx
   mov %esp, %ecx
   //clear direction flag in efl
   cld
   movw $0x10, %ax                 // set kernel data segment registers
   mov %ax, %ds                    // set kernel data segment registers
   mov %ax, %es                    // set kernel data segment registers
   leal kernel_stack + 8192, %esp  // point esp to the kernel stack
   //push ecx (onto the kernel stack)
   push %ecx
   //clear the direction flag of efl
   cld
   //call the Kernel C code
   call CNAME(kernel)

