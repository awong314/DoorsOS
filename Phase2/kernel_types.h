// kernel_types.h, 159

#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

#include "kernel_constants.h"

typedef void (*func_p_t)(); // void-return function pointer type

typedef enum {AVAIL, READY, RUN, SLEEP} state_t;   //SLEEP state added in Phase2

typedef struct {
   unsigned int regs[4];     // changed from regs[8] from Phase1
   unsigned int ebx,
		edx,
		ecx,
		eax,
		intr_num,
		eip,
		cs,
		efl;
} trapframe_t;

typedef struct {
   //Phase1
   state_t state;            // state of process
   int runtime;              // runtime of this run
   int totaltime;            // total runtime
   trapframe_t *trapframe_p; // points to saved trapframe
   //Phase2
   int wake_time;	     // wake time of process given in centi-seconds = current OS time + sleep time requested
} pcb_t;                     

typedef struct {             // generic queue type
   int q[Q_SIZE];            // integers are queued in q[] array
   int size;                 // size is also where the tail is for new data
   int indexOfFirst;         // index of the first value
} pid_q_t;

#endif // _KERNEL_TYPES_H_
